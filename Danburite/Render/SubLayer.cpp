#include "SubLayer.h"

namespace Render
{
	SubLayer::SubLayer(
		VK::Device &device,
		VK::DescriptorSetLayout &descSetLayout,
		Infra::DeferredDeleter &deferredDeleter,
		Dev::SCBBuilder &scbBuilder,
		Dev::DescriptorUpdater &descUpdater,
		ResourcePool &resourcePool,
		GlobalDescriptorManager &globalDescManager,
		Renderer const *const pRenderer) noexcept :
		__device				{ device },
		__descSetLayout			{ descSetLayout },
		__deferredDeleter		{ deferredDeleter },
		__scbBuilder			{ scbBuilder },
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
			Infra::EventListener<RenderObject const *, bool>::bind(
				&SubLayer::__onObjectDrawableChanged, this, std::placeholders::_1, std::placeholders::_2);

		__pObjectNeedRedrawListener =
			Infra::EventListener<RenderObject const *>::bind(
				&SubLayer::__onObjectNeedRedraw, this);

		__pMeshVertexAttribFlagsChangeListener =
			Infra::EventListener<Mesh const *, uint32_t, uint32_t>::bind(
				&SubLayer::__onMeshVertexAttribFlagsChanged, this);

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
		VK::ImageView &colorAttachment,
		VK::ImageView *const pDepthStencilAttachment,
		RendererResourceManager &rendererResourceManager,
		VkRect2D const &renderArea) const
	{
		if (__drawSequence.empty())
			return;

		auto &renderPass	{ rendererResourceManager.getRenderPassOf(__pRenderer) };
		auto &framebuffer	{ rendererResourceManager.getFramebufferOf(__pRenderer, colorAttachment, pDepthStencilAttachment) };
		auto &pipeline		{ rendererResourceManager.getPipelineOf(__pRenderer) };

		size_t const drawSequenceLength		{ __drawSequence.size() };
		size_t const concurrencyCount		{ __scbBuilder.getCapacity() };
		size_t const seqUnitLength			{ drawSequenceLength / concurrencyCount };
		size_t const seqRemainder			{ drawSequenceLength % concurrencyCount };

		std::vector<std::future<VK::CommandBuffer *>> subDrawExecutions;
		std::vector<VkCommandBuffer> secondaryBufferHandles;

		__beginRenderPass(cmdBuffer, renderPass, framebuffer, renderArea);

		size_t sequenceBegin{ };
		for (size_t coreIter{ }; coreIter < concurrencyCount; ++coreIter)
		{
			size_t sequenceEnd{ sequenceBegin + seqUnitLength };
			if (coreIter < seqRemainder)
				++sequenceEnd;

			if (sequenceBegin >= sequenceEnd)
				break;

			subDrawExecutions.emplace_back(
				__subDraw(renderPass, framebuffer, pipeline, sequenceBegin, sequenceEnd));

			sequenceBegin = sequenceEnd;
		}

		for (auto &secondaryBuffer : subDrawExecutions)
			secondaryBufferHandles.emplace_back(secondaryBuffer.get()->getHandle());

		cmdBuffer.vkCmdExecuteCommands(
			static_cast<uint32_t>(secondaryBufferHandles.size()),
			secondaryBufferHandles.data());

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

		if (__drawSequenceInvalidated)
			__validateDrawSequence();

		__instanceInfoBufferInvalidated = false;
		__drawSequenceInvalidated = false;
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
		pObject->getNeedRedrawEvent() += __pObjectNeedRedrawListener;

		uint32_t const instanceCount{ pObject->getInstanceCount() };
		__object2Region[pObject] = std::make_unique<Infra::Region>(__objectRegionAllocator, instanceCount, 1ULL);

		__registerMesh(pObject, pObject->getMesh());

		if (__pRenderer->useMaterial())
		{
			pObject->getMaterialChangeEvent() += __pObjectMaterialChangeListener;

			for (uint32_t instanceIter{ }; instanceIter < instanceCount; ++instanceIter)
			{
				for (const auto pMaterial : pObject->getMaterialPackOf(instanceIter))
					__registerMaterial(pMaterial);
			}

			__validateInstanceInfoHostBuffer(pObject);
			__instanceInfoBufferInvalidated = true;
		}

		__drawSequenceInvalidated = true;
		_invalidate();

		__needRedrawEvent.invoke(this);
	}

	void SubLayer::__unregisterObject(
		RenderObject const *pObject)
	{
		pObject->getMeshChangeEvent() -= __pObjectMeshChangeListener;
		pObject->getInstanceCountChangeEvent() -= __pObjectInstanceCountChangeListener;
		pObject->getNeedRedrawEvent() -= __pObjectNeedRedrawListener;

		__object2Region.erase(pObject);

		auto const pMesh{ pObject->getMesh() };
		if (pMesh)
			__unregisterMesh(pObject, pMesh);

		if (__pRenderer->useMaterial())
		{
			pObject->getMaterialChangeEvent() -= __pObjectMaterialChangeListener;

			for (uint32_t instanceIter{ }; instanceIter < pObject->getInstanceCount(); ++instanceIter)
			{
				for (const auto pMaterial : pObject->getMaterialPackOf(instanceIter))
					__unregisterMaterial(pMaterial);
			}
		}

		__drawSequenceInvalidated = true;
		_invalidate();

		__needRedrawEvent.invoke(this);
	}

	void SubLayer::__registerMesh(
		RenderObject const *const pObject,
		Mesh const *const pMesh)
	{
		pMesh->getVertexAttribFlagsChangeEvent() += __pMeshVertexAttribFlagsChangeListener;
		__mesh2Objects[pMesh].emplace(pObject);
	}

	void SubLayer::__unregisterMesh(
		RenderObject const *const pObject,
		Mesh const *const pMesh)
	{
		pMesh->getVertexAttribFlagsChangeEvent() -= __pMeshVertexAttribFlagsChangeListener;

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

			for (auto const pMaterial : pObject->getMaterialPackOf(instanceIter))
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

		__descUpdater.addInfos(
			__getDescSet(), Constants::SUB_LAYER_DESC_INSTANCE_INFO_LOCATION,
			0U, 1U, VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, &bufferInfo);
	}

	void SubLayer::__validateDrawSequence()
	{
		__drawSequence.clear();

		std::vector<Mesh const *> sortedMeshes;

		for (auto const &[pMesh, _] : __mesh2Objects)
			sortedMeshes.emplace_back(pMesh);

		std::sort(sortedMeshes.begin(), sortedMeshes.end(), [] (auto const lhs, auto const rhs)
		{
			return (lhs->getVertexAttribFlags() < rhs->getVertexAttribFlags());
		});

		for (auto const pMesh : sortedMeshes)
		{
			for (auto const pObject : __mesh2Objects.at(pMesh))
			{
				uint32_t const baseId
				{
					static_cast<uint32_t>(
						__object2Region.at(pObject)->getOffset())
				};

				__drawSequence.emplace_back(pMesh, baseId, pObject);
			}
		}
	}

	void SubLayer::__onObjectMeshChanged(
		RenderObject const *const pObject,
		Mesh const *const pPrev,
		Mesh const *const pCur)
	{
		__unregisterMesh(pObject, pPrev);

		if (!pCur)
			return;

		__registerMesh(pObject, pCur);
		__drawSequenceInvalidated = true;
		_invalidate();
	}

	void SubLayer::__onObjectMaterialChanged(
		RenderObject const *const pObject,
		uint32_t const instanceIndex,
		std::type_index const &type,
		Material const *const pPrev,
		Material const *const pCur)
	{
		if (pPrev)
			__unregisterMaterial(pPrev);

		if (pCur)
		{
			__registerMaterial(pCur);

			__validateInstanceInfoHostBuffer(pObject, instanceIndex, type, pCur);
			__instanceInfoBufferInvalidated = true;
			_invalidate();
		}
	}

	void SubLayer::__onObjectInstanceCountChanged(
		RenderObject const *const pObject,
		uint32_t const prev,
		uint32_t const cur)
	{
		auto &pRegion{ __object2Region.at(pObject) };
		pRegion.reset();
		pRegion = std::make_unique<Infra::Region>(__objectRegionAllocator, cur, 1ULL);

		__drawSequenceInvalidated = true;
		_invalidate();
	}

	void SubLayer::__onObjectDrawableChanged(
		RenderObject const *const pObject,
		bool const cur)
	{
		if (cur)
			__registerObject(pObject);
		else
			__unregisterObject(pObject);
	}

	void SubLayer::__onMeshVertexAttribFlagsChanged()
	{
		__drawSequenceInvalidated = true;
		_invalidate();
	}

	void SubLayer::__onObjectNeedRedraw()
	{
		__needRedrawEvent.invoke(this);
	}

	void SubLayer::__beginRenderPass(
		VK::CommandBuffer &cmdBuffer,
		VK::RenderPass &renderPass,
		VK::Framebuffer &framebuffer,
		VkRect2D const &renderArea) const
	{
		VkRenderPassBeginInfo renderPassBeginInfo{ };
		renderPassBeginInfo.sType			= VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass		= renderPass.getHandle();
		renderPassBeginInfo.framebuffer		= framebuffer.getHandle();
		renderPassBeginInfo.renderArea		= renderArea;

		VkSubpassBeginInfo const subpassBeginInfo
		{
			.sType		{ VkStructureType::VK_STRUCTURE_TYPE_SUBPASS_BEGIN_INFO },
			.contents	{ VkSubpassContents::VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS }
		};

		VkSubpassEndInfo const subpassEndInfo
		{
			.sType		{ VkStructureType::VK_STRUCTURE_TYPE_SUBPASS_END_INFO }
		};

		cmdBuffer.vkCmdBeginRenderPass2(&renderPassBeginInfo, &subpassBeginInfo);
	}

	void SubLayer::__bindDescSets(
		VK::CommandBuffer &cmdBuffer) const
	{
		std::array const descSets
		{
			// MATERIALS_DESC_SET_LOCATION
			__globalDescManager.getMaterialsDescSet(),

			// SAMPLED_IMAGES_DESC_SET_LOCATION
			__globalDescManager.getSampledImagesDescSet(),

			// SUB_LAYER_DESC_SET_LOCATION
			__getDescSet(),

			// RENDERER_DESC_SET_LOCATION
			__pRenderer->getDescSet()
		};

		uint32_t descCount{ static_cast<uint32_t>(descSets.size()) };
		if (!(descSets.back()))
			--descCount;

		cmdBuffer.vkCmdBindDescriptorSets(
			VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS,
			__pRenderer->getPipelineLayout().getHandle(),
			0U, descCount, descSets.data(), 0U, nullptr);
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

	std::future<VK::CommandBuffer *> SubLayer::__subDraw(
		VK::RenderPass const &renderPass,
		VK::Framebuffer const &framebuffer,
		VK::Pipeline const &pipeline,
		size_t const sequenceBegin,
		size_t const sequenceEnd) const
	{
		return __scbBuilder.build(
			[this, &renderPass, &framebuffer, &pipeline, sequenceBegin, sequenceEnd] (auto &secondaryBuffer)
		{
			__beginSecondaryBuffer(secondaryBuffer, renderPass, framebuffer);

			secondaryBuffer.vkCmdBindPipeline(
				VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS,
				pipeline.getHandle());

			if (__pRenderer->useMaterial())
				__bindDescSets(secondaryBuffer);

			Mesh const *pBoundMesh{ };
			uint32_t boundVertexAttribFlags{ };

			for (size_t seqIter{ sequenceBegin }; seqIter < sequenceEnd; ++seqIter)
			{
				auto const &[pMesh, baseId, pObject] { __drawSequence[seqIter] };
				if (pBoundMesh != pMesh)
				{
					pBoundMesh = pMesh;
					pMesh->bind(secondaryBuffer);

					if (boundVertexAttribFlags != pMesh->getVertexAttribFlags())
					{
						boundVertexAttribFlags = pMesh->getVertexAttribFlags();
						__pRenderer->bindVertexInput(secondaryBuffer, boundVertexAttribFlags);
					}
				}

				pObject->draw(secondaryBuffer, baseId);
			}

			secondaryBuffer.vkEndCommandBuffer();
		});
	}

	void SubLayer::__beginSecondaryBuffer(
		VK::CommandBuffer &secondaryBuffer,
		VK::RenderPass const &renderPass,
		VK::Framebuffer const &framebuffer)
	{
		VkCommandBufferInheritanceInfo const inheritanceInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO },
			.renderPass		{ renderPass.getHandle() },
			.subpass		{ 0U },
			.framebuffer	{ framebuffer.getHandle() }
		};

		VkCommandBufferBeginInfo const commandBufferBeginInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO },
			.flags				{
				VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT |
				VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT
			},
			.pInheritanceInfo	{ &inheritanceInfo }
		};

		secondaryBuffer.vkBeginCommandBuffer(&commandBufferBeginInfo);
	}
}