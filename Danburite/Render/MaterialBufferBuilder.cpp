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

	void MaterialBufferBuilder::addMaterial(
		Material const *const pMaterial)
	{
		auto &[ref, id] { __materialRefIdMap[pMaterial] };
		if (!ref)
		{
			id = __materialIdAllocator.allocate();
			__registerMaterial(pMaterial);
		}

		++ref;
	}

	void MaterialBufferBuilder::removeMaterial(
		Material const *const pMaterial)
	{
		auto &[ref, id] { __materialRefIdMap[pMaterial] };
		--ref;

		if (!ref)
		{
			__unregisterMaterial(pMaterial);
			__materialIdAllocator.free(id);
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
		if (!__materialBufferInvalidated)
			return;

		__validateMaterialBuffer();

		__materialBufferInvalidated = false;
	}

	void MaterialBufferBuilder::__registerMaterial(
		Material const *const pMaterial)
	{
		pMaterial->getUpdateEvent() += __pMaterialUpdateListener;

		__validateMaterialHostBuffer(pMaterial);
		__materialBufferInvalidated = true;

		_invalidate();
	}

	void MaterialBufferBuilder::__unregisterMaterial(
		Material const *const pMaterial)
	{
		pMaterial->getUpdateEvent() -= __pMaterialUpdateListener;
		__materialRefIdMap.erase(pMaterial);
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
	}

	void MaterialBufferBuilder::__onMaterialUpdated(
		Material const *pMaterial) noexcept
	{
		__validateMaterialHostBuffer(pMaterial);
		__materialBufferInvalidated = true;
		_invalidate();
	}
}