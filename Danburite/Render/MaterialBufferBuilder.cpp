#include "MaterialBufferBuilder.h"

namespace Render
{
	MaterialBufferBuilder::MaterialBufferBuilder(
		ResourcePool &resourcePool) noexcept :
		__resourcePool	{ resourcePool }
	{
		__pMaterialUpdateListener = Infra::EventListener<Material const *>::bind(
			&MaterialBufferBuilder::__onMaterialUpdated, this, std::placeholders::_1);
	}

	MaterialBufferBuilder::~MaterialBufferBuilder() noexcept
	{
		if (__pMaterialBuffer)
		{
			__resourcePool.recycleBuffer(
				ResourcePool::BufferType::HOST_VISIBLE_COHERENT_STORAGE,
				std::move(__pMaterialBuffer));
		}
	}

	void MaterialBufferBuilder::registerMaterial(
		Material const *const pMaterial)
	{
		auto &[ref, id] { __materialRefIdMap[pMaterial] };
		if (!ref)
		{
			pMaterial->getUpdateEvent() += __pMaterialUpdateListener;

			id = __materialIdAllocator.allocate();

			__validateMaterialHostBuffer(pMaterial);
			__materialBufferInvalidated = true;

			_invalidate();
		}

		++ref;
	}

	void MaterialBufferBuilder::unregisterMaterial(
		Material const *const pMaterial)
	{
		auto &[ref, id] { __materialRefIdMap[pMaterial] };
		--ref;

		if (!ref)
		{
			pMaterial->getUpdateEvent() -= __pMaterialUpdateListener;

			__materialIdAllocator.free(id);
			__materialRefIdMap.erase(pMaterial);
		}
	}

	uint32_t MaterialBufferBuilder::getIdOf(
		Material const *const pMaterial) const noexcept
	{
		return __materialRefIdMap.at(pMaterial).second;
	}

	Dev::MemoryBuffer const &MaterialBufferBuilder::getMaterialBuffer() const noexcept
	{
		return *__pMaterialBuffer;
	}

	void MaterialBufferBuilder::_onValidate()
	{
		__validateMaterialBuffer();
	}

	void MaterialBufferBuilder::__validateMaterialHostBuffer(
		Material const *const pMaterial) noexcept
	{
		uint32_t const materialId	{ __materialRefIdMap.at(pMaterial).second };
		size_t const materialSize	{ pMaterial->getSize() };
		size_t const memOffset		{ materialId * materialSize };

		if (memOffset >= __materialHostBuffer.getSize())
			__materialHostBuffer.resize(memOffset + materialSize);

		__materialHostBuffer.set(memOffset, pMaterial->getData(), materialSize);
	}

	void MaterialBufferBuilder::__validateMaterialBuffer()
	{
		if (!__materialBufferInvalidated)
			return;

		if (__pMaterialBuffer)
		{
			__resourcePool.recycleBuffer(
				ResourcePool::BufferType::HOST_VISIBLE_COHERENT_STORAGE,
				std::move(__pMaterialBuffer));
		}

		size_t const bufferSize{ __materialHostBuffer.getSize() };

		__pMaterialBuffer = __resourcePool.getBuffer(
			ResourcePool::BufferType::HOST_VISIBLE_COHERENT_STORAGE,
			bufferSize);

		std::memcpy(__pMaterialBuffer->getData(), __materialHostBuffer.getData(), bufferSize);

		__materialBufferInvalidated = false;
	}

	void MaterialBufferBuilder::__onMaterialUpdated(
		Material const *pMaterial) noexcept
	{
		__validateMaterialHostBuffer(pMaterial);
		__materialBufferInvalidated = true;
		_invalidate();
	}
}