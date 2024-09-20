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
		if (__pBuffer)
		{
			__resourcePool.recycleBuffer(
				ResourcePool::BufferType::HOST_VISIBLE_COHERENT_STORAGE,
				std::move(__pBuffer));
		}
	}

	void MaterialBufferBuilder::addMaterial(
		Material const *const pMaterial)
	{
		auto &[ref, id] { __refIdMap[pMaterial] };
		if (!ref)
		{
			id = __idAllocator.allocate();
			__registerMaterial(pMaterial);
		}

		++ref;
	}

	void MaterialBufferBuilder::removeMaterial(
		Material const *const pMaterial)
	{
		auto &[ref, id] { __refIdMap[pMaterial] };
		--ref;

		if (!ref)
		{
			__unregisterMaterial(pMaterial);
			__idAllocator.free(id);
		}
	}

	uint32_t MaterialBufferBuilder::getIdOf(
		Material const *const pMaterial) const noexcept
	{
		return __refIdMap.at(pMaterial).second;
	}

	Dev::MemoryBuffer const &MaterialBufferBuilder::getMaterialBuffer() const noexcept
	{
		return *__pBuffer;
	}

	void MaterialBufferBuilder::_onValidate()
	{
		__validateMaterialBuffer();
	}

	void MaterialBufferBuilder::__registerMaterial(
		Material const *const pMaterial)
	{
		pMaterial->getUpdateEvent() += __pMaterialUpdateListener;
		__validateMaterialHostBuffer(pMaterial);
		_invalidate();
	}

	void MaterialBufferBuilder::__unregisterMaterial(
		Material const *const pMaterial)
	{
		pMaterial->getUpdateEvent() -= __pMaterialUpdateListener;
		__refIdMap.erase(pMaterial);
	}

	void MaterialBufferBuilder::__validateMaterialHostBuffer(
		Material const *const pMaterial) noexcept
	{
		uint32_t const materialId	{ __refIdMap.at(pMaterial).second };
		size_t const materialSize	{ pMaterial->getSize() };
		size_t const memOffset		{ materialId * materialSize };

		if (memOffset >= __hostBuffer.getSize())
			__hostBuffer.resize(memOffset + materialSize);

		__hostBuffer.set(memOffset, pMaterial->getData(), materialSize);
	}

	void MaterialBufferBuilder::__validateMaterialBuffer()
	{
		if (__pBuffer)
		{
			__resourcePool.recycleBuffer(
				ResourcePool::BufferType::HOST_VISIBLE_COHERENT_STORAGE,
				std::move(__pBuffer));
		}

		size_t const bufferSize{ __hostBuffer.getSize() };

		__pBuffer = __resourcePool.getBuffer(
			ResourcePool::BufferType::HOST_VISIBLE_COHERENT_STORAGE,
			bufferSize);

		std::memcpy(__pBuffer->getData(), __hostBuffer.getData(), bufferSize);
	}

	void MaterialBufferBuilder::__onMaterialUpdated(
		Material const *pMaterial) noexcept
	{
		__validateMaterialHostBuffer(pMaterial);
		_invalidate();
	}
}