#include "SubLayer.h"

namespace Render
{
	SubLayer::SubLayer(
		VK::Device &device,
		VK::DescriptorSetLayout &descSetLayout,
		Infra::DeferredDeleter &deferredDeleter,
		Dev::DescriptorUpdater &descUpdater,
		ResourcePool &resourcePool,
		GlobalDescriptorManager &globalDescManager,
		Renderer const *const pRenderer) noexcept :
		__device				{ device },
		__descSetLayout			{ descSetLayout },
		__deferredDeleter		{ deferredDeleter },
		__descUpdater			{ descUpdater },
		__resourcePool			{ resourcePool },
		__globalDescManager		{ globalDescManager },
		__pRenderer				{ pRenderer }
	{
		__pObjectMeshChangeListener =
			Infra::EventListener<RenderObject const *, Mesh const *, Mesh const *>::bind(
				&SubLayer::__onObjectMeshChanged, this,
				std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

		__pObjectInstanceCountChangeListener =
			Infra::EventListener<RenderObject const *, uint32_t, uint32_t>::bind(
				&SubLayer::__onObjectInstanceCountChanged, this,
				std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

		__pObjectDrawableChangeListener =
			Infra::EventListener<const RenderObject *, bool>::bind(
				&SubLayer::__onObjectDrawableChanged, this, std::placeholders::_1, std::placeholders::_2);

		if (__pRenderer->useMaterial())
		{
			__createDescPool();
			__allocDescSets();

			__pObjectMaterialChangeListener =
				Infra::EventListener<RenderObject const *, uint32_t, std::type_index, Material const *, Material const *>::bind(
					&SubLayer::__onObjectMaterialChanged, this,
					std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
		}
	}

	SubLayer::~SubLayer() noexcept
	{
		if (__pInstanceInfoBuffer)
		{
			__resourcePool.recycleBuffer(
				ResourcePool::BufferType::HOST_VISIBLE_COHERENT_STORAGE,
				std::move(__pInstanceInfoBuffer));
		}

		if (__pDescPool)
			__deferredDeleter.reserve(std::move(__pDescPool));
	}

	void SubLayer::addRenderObject(
		RenderObject const *const pObject) noexcept
	{
		pObject->getDrawableChangeEvent() += __pObjectDrawableChangeListener;
		if (pObject->isDrawable())
			__registerObject(pObject);
	}

	void SubLayer::removeRenderObject(
		RenderObject const *const pObject) noexcept
	{
		pObject->getDrawableChangeEvent() -= __pObjectDrawableChangeListener;
		if (pObject->isDrawable())
			__unregisterObject(pObject);
	}

	bool SubLayer::isEmpty() const noexcept
	{
		return __object2Region.empty();
	}

	void SubLayer::draw(
		VK::CommandBuffer &cmdBuffer,
		VkDescriptorSet const hGlobalDescSet,
		VK::ImageView &outputAttachment,
		RendererResourceManager &rendererResourceManager,
		VkRect2D const &renderArea) const
	{
		__beginRenderPass(
			cmdBuffer, outputAttachment,
			rendererResourceManager, renderArea);

		__bindPipeline(cmdBuffer, rendererResourceManager);

		if (__pRenderer->useMaterial())
			__bindDescSets(cmdBuffer, hGlobalDescSet);

		Mesh const *pBoundMesh{ };
		for (auto const &[pMesh, objects] : __mesh2Objects)
		{
			if (pBoundMesh != pMesh)
			{
				pBoundMesh = pMesh;
				pMesh->bind(cmdBuffer);
			}

			for (auto const pObject : objects)
			{
				uint32_t const baseId
				{
					static_cast<uint32_t>(
						__object2Region.at(pObject)->getOffset())
				};

				pObject->draw(cmdBuffer, baseId);
			}
		}

		__endRenderPass(cmdBuffer);
	}

	void SubLayer::_onValidate()
	{
		if (isEmpty())
			return;

		if (__instanceInfoBufferInvalidated)
		{
			__validateInstanceInfoBuffer();
			__validateDescSet();
		}

		__instanceInfoBufferInvalidated = false;
	}

	void SubLayer::__createDescPool()
	{
		uint32_t const descSetCount{ static_cast<uint32_t>(__descSets.size()) };

		std::vector<VkDescriptorPoolSize> poolSizes;
		poolSizes.emplace_back(
			VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			descSetCount);

		VkDescriptorPoolCreateInfo const createInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO },
			.maxSets		{ descSetCount },
			.poolSizeCount	{ static_cast<uint32_t>(poolSizes.size()) },
			.pPoolSizes		{ poolSizes.data() }
		};

		__pDescPool = std::make_shared<VK::DescriptorPool>(__device, createInfo);
	}

	void SubLayer::__allocDescSets()
	{
		uint32_t const descSetCount{ static_cast<uint32_t>(__descSets.size()) };

		std::vector<VkDescriptorSetLayout> layoutHandles;

		for (uint32_t iter{ }; iter < descSetCount; ++iter)
			layoutHandles.emplace_back(__descSetLayout.getHandle());

		VkDescriptorSetAllocateInfo const allocInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO },
			.descriptorPool		{ __pDescPool->getHandle() },
			.descriptorSetCount	{ descSetCount },
			.pSetLayouts		{ layoutHandles.data() }
		};

		__device.vkAllocateDescriptorSets(&allocInfo, __descSets.data());
	}

	void SubLayer::__registerObject(
		RenderObject const *const pObject)
	{
		pObject->getMeshChangeEvent() += __pObjectMeshChangeListener;
		pObject->getInstanceCountChangeEvent() += __pObjectInstanceCountChangeListener;

		uint32_t const instanceCount{ pObject->getInstanceCount() };
		__object2Region[pObject] = std::make_unique<Infra::Region>(__objectRegionAllocator, instanceCount, 1ULL);

		__registerMesh(pObject, pObject->getMesh().get());

		if (__pRenderer->useMaterial())
		{
			pObject->getMaterialChangeEvent() += __pObjectMaterialChangeListener;

			for (uint32_t instanceIter{ }; instanceIter < instanceCount; ++instanceIter)
			{
				for (const auto pMaterial : pObject->getMaterialPack(instanceIter))
					__registerMaterial(pMaterial);
			}

			__validateInstanceInfoHostBuffer(pObject);
			__instanceInfoBufferInvalidated = true;
			_invalidate();
		}

		__needRedrawEvent.invoke(this);
	}

	void SubLayer::__unregisterObject(
		RenderObject const *pObject)
	{
		pObject->getMeshChangeEvent() -= __pObjectMeshChangeListener;
		pObject->getInstanceCountChangeEvent() -= __pObjectInstanceCountChangeListener;

		__object2Region.erase(pObject);

		auto const &pMesh{ pObject->getMesh() };
		if (pMesh)
			__unregisterMesh(pObject, pMesh.get());

		if (__pRenderer->useMaterial())
		{
			pObject->getMaterialChangeEvent() -= __pObjectMaterialChangeListener;

			for (uint32_t instanceIter{ }; instanceIter < pObject->getInstanceCount(); ++instanceIter)
			{
				for (const auto pMaterial : pObject->getMaterialPack(instanceIter))
					__unregisterMaterial(pMaterial);
			}
		}

		__needRedrawEvent.invoke(this);
	}

	void SubLayer::__registerMesh(
		RenderObject const *const pObject,
		Mesh const *const pMesh)
	{
		__mesh2Objects[pMesh].emplace(pObject);
	}

	void SubLayer::__unregisterMesh(
		RenderObject const *const pObject,
		Mesh const *const pMesh)
	{
		auto &objects{ __mesh2Objects.at(pMesh) };
		objects.erase(pObject);

		if (objects.empty())
			__mesh2Objects.erase(pMesh);
	}

	void SubLayer::__registerMaterial(
		Material const *const pMaterial)
	{
		__globalDescManager.addMaterial(pMaterial);
	}

	void SubLayer::__unregisterMaterial(
		Material const *const pMaterial)
	{
		__globalDescManager.removeMaterial(pMaterial);
	}

	void SubLayer::__validateInstanceInfoHostBuffer(
		RenderObject const *pObject)
	{
		auto const &pRegion{ __object2Region.at(pObject) };

		const uint32_t instanceCount	{ pObject->getInstanceCount() };
		const size_t baseId				{ pRegion->getOffset() };
		const size_t lastId				{ baseId + instanceCount };

		if (lastId >= __instanceInfoHostBuffer.size())
			__instanceInfoHostBuffer.resize(lastId);

		for (uint32_t instanceIter{ }; instanceIter < instanceCount; ++instanceIter)
		{
			auto &instanceInfo{ __instanceInfoHostBuffer[baseId + instanceIter] };
			instanceInfo.reset();

			for (auto const pMaterial : pObject->getMaterialPack(instanceIter))
			{
				std::type_index const materialType{ typeid(*pMaterial) };

				auto const slot{ __pRenderer->getMaterialSlotOf(materialType) };
				if (slot.has_value())
				{
					instanceInfo.materialIds[slot.value()] =
						__globalDescManager.getMaterialIdOf(pMaterial);
				}
			}
		}
	}

	void SubLayer::__validateInstanceInfoHostBuffer(
		RenderObject const *const pObject,
		uint32_t const instanceIndex,
		std::type_index const &materialType,
		Material const *const pMaterial)
	{
		auto const &pRegion{ __object2Region.at(pObject) };

		const size_t baseId			{ pRegion->getOffset() };
		const size_t instanceId		{ baseId + instanceIndex };

		if (instanceId >= __instanceInfoHostBuffer.size())
			__instanceInfoHostBuffer.resize(instanceId);

		auto &instanceInfo{ __instanceInfoHostBuffer[instanceId] };

		auto const slot{ __pRenderer->getMaterialSlotOf(materialType) };
		if (slot.has_value())
		{
			instanceInfo.materialIds[slot.value()] =
				__globalDescManager.getMaterialIdOf(pMaterial);
		}
	}

	void SubLayer::__validateInstanceInfoBuffer()
	{
		size_t const bufferSize{ __instanceInfoHostBuffer.size() * sizeof(InstanceInfo) };

		if (__pInstanceInfoBuffer)
		{
			__resourcePool.recycleBuffer(
				ResourcePool::BufferType::HOST_VISIBLE_COHERENT_STORAGE,
				std::move(__pInstanceInfoBuffer));
		}

		__pInstanceInfoBuffer = __resourcePool.getBuffer(
			ResourcePool::BufferType::HOST_VISIBLE_COHERENT_STORAGE,
			bufferSize);

		std::memcpy(__pInstanceInfoBuffer->getData(), __instanceInfoHostBuffer.data(), bufferSize);
	}

	void SubLayer::__validateDescSet()
	{
		__advanceDescSet();

		VkDescriptorBufferInfo const bufferInfo
		{
			.buffer	{ __pInstanceInfoBuffer->getHandle() },
			.range	{ __pInstanceInfoBuffer->getSize() }
		};

		__descUpdater.addBufferInfos(
			__getDescSet(), Constants::SUB_LAYER_DESC_SET_LOCATION,
			0U, 1U, VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, &bufferInfo);
	}

	void SubLayer::__onObjectMeshChanged(
		RenderObject const *const pObject,
		Mesh const *const pPrev,
		Mesh const *const pCur) noexcept
	{
		__unregisterMesh(pObject, pPrev);

		if (pCur)
		{
			__registerMesh(pObject, pCur);
			__needRedrawEvent.invoke(this);
		}
	}

	void SubLayer::__onObjectMaterialChanged(
		RenderObject const *const pObject,
		uint32_t const instanceIndex,
		std::type_index const &type,
		Material const *const pPrev,
		Material const *const pCur) noexcept
	{
		if (pPrev)
			__unregisterMaterial(pPrev);

		if (pCur)
		{
			__registerMaterial(pCur);

			__validateInstanceInfoHostBuffer(pObject, instanceIndex, type, pCur);
			__instanceInfoBufferInvalidated = true;
			_invalidate();

			__needRedrawEvent.invoke(this);
		}
	}

	void SubLayer::__onObjectInstanceCountChanged(
		RenderObject const *const pObject,
		uint32_t const prev,
		uint32_t const cur) noexcept
	{
		auto &pRegion{ __object2Region.at(pObject) };
		pRegion.reset();
		pRegion = std::make_unique<Infra::Region>(__objectRegionAllocator, cur, 1ULL);

		__needRedrawEvent.invoke(this);
	}

	void SubLayer::__onObjectDrawableChanged(
		RenderObject const *const pObject,
		bool const cur) noexcept
	{
		if (cur)
			__registerObject(pObject);
		else
			__unregisterObject(pObject);
	}

	void SubLayer::__beginRenderPass(
		VK::CommandBuffer &cmdBuffer,
		VK::ImageView &outputAttachment,
		RendererResourceManager &rendererResourceManager,
		VkRect2D const &renderArea) const
	{
		auto &renderPass	{ rendererResourceManager.getRenderPassOf(__pRenderer) };
		auto &framebuffer	{ rendererResourceManager.getFramebufferOf(__pRenderer, outputAttachment) };

		VkRenderPassBeginInfo renderPassBeginInfo{ };
		renderPassBeginInfo.sType			= VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass		= renderPass.getHandle();
		renderPassBeginInfo.framebuffer		= framebuffer.getHandle();

		VkSubpassBeginInfo const subpassBeginInfo
		{
			.sType		{ VkStructureType::VK_STRUCTURE_TYPE_SUBPASS_BEGIN_INFO },
			.contents	{ VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE }
		};

		VkSubpassEndInfo const subpassEndInfo
		{
			.sType		{ VkStructureType::VK_STRUCTURE_TYPE_SUBPASS_END_INFO }
		};

		cmdBuffer.vkCmdBeginRenderPass2(&renderPassBeginInfo, &subpassBeginInfo);
	}

	void SubLayer::__bindPipeline(
		VK::CommandBuffer &cmdBuffer,
		RendererResourceManager &rendererResourceManager) const
	{
		auto &pipeline{ rendererResourceManager.getPipelineOf(__pRenderer) };
		
		cmdBuffer.vkCmdBindPipeline(
			VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipeline.getHandle());
	}

	void SubLayer::__bindDescSets(
		VK::CommandBuffer &cmdBuffer,
		VkDescriptorSet const hGlobalDescSet) const
	{
		std::array<VkDescriptorSet, 2ULL> descSets{ };
		descSets[Constants::GLOBAL_DESC_SET_LOCATION]		= hGlobalDescSet;
		descSets[Constants::SUB_LAYER_DESC_SET_LOCATION]	= __getDescSet();

		cmdBuffer.vkCmdBindDescriptorSets(
			VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS,
			__pRenderer->getPipelineLayout().getHandle(),
			0U, static_cast<uint32_t>(descSets.size()), descSets.data(),
			0U, nullptr);
	}

	void SubLayer::__endRenderPass(
		VK::CommandBuffer &cmdBuffer) const
	{
		VkSubpassEndInfo const subpassEndInfo
		{
			.sType	{ VkStructureType::VK_STRUCTURE_TYPE_SUBPASS_END_INFO }
		};

		cmdBuffer.vkCmdEndRenderPass2(&subpassEndInfo);
	}
}