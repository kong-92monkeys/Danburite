#include "Layer.h"

namespace Render
{
	Layer::Layer(
		VK::Device &device,
		VK::DescriptorSetLayout &layerDescSetLayout,
		VK::DescriptorSetLayout &subLayerDescSetLayout,
		Infra::DeferredDeleter &deferredDeleter,
		Dev::SCBBuilder &scbBuilder,
		Dev::DescriptorUpdater &descUpdater,
		ResourcePool &resourcePool,
		GlobalDescriptorManager &globalDescManager) noexcept :
		__device				{ device },
		__layerDescSetLayout	{ layerDescSetLayout },
		__subLayerDescSetLayout	{ subLayerDescSetLayout },
		__deferredDeleter		{ deferredDeleter },
		__scbBuilder			{ scbBuilder },
		__descUpdater			{ descUpdater },
		__resourcePool			{ resourcePool },
		__globalDescManager		{ globalDescManager }
	{
		__pObjectRendererChangeListener =
			Infra::EventListener<RenderObject const *, Renderer const *, Renderer const *>::bind(
				&Layer::__onObjectRendererChanged, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

		__pRendererPriorityChangeListener =
			Infra::EventListener<Renderer const *, int, int>::bind(
				&Layer::__onRendererPriorityChanged, this);

		__pSubLayerInvalidatedListener =
			Infra::EventListener<SubLayer *>::bind(
				&Layer::__onSubLayerInvalidated, this, std::placeholders::_1);

		__pSubLayerNeedRedrawListener =
			Infra::EventListener<SubLayer const *>::bind(
				&Layer::__onSubLayerRedrawNeeded, this);

		__createDescPool();
		__allocDescSets();
	}

	Layer::~Layer() noexcept
	{
		if (__pDataBuffer)
		{
			__resourcePool.recycleBuffer(
				ResourcePool::BufferType::HOST_VISIBLE_COHERENT_STORAGE,
				std::move(__pDataBuffer));
		}

		__deferredDeleter.reserve(std::move(__pDescPool));
	}

	void Layer::setPriority(
		int const priority) noexcept
	{
		if (__priority == priority)
			return;

		int const prevPriority{ __priority };
		__priority = priority;

		__priorityChangeEvent.invoke(this, prevPriority, priority);
	}

	void Layer::setData(
		void const *const pData,
		size_t const size)
	{
		if (__pDataBuffer)
		{
			__resourcePool.recycleBuffer(
				ResourcePool::BufferType::HOST_VISIBLE_COHERENT_STORAGE,
				std::move(__pDataBuffer));
		}

		__pDataBuffer = __resourcePool.getBuffer(
			ResourcePool::BufferType::HOST_VISIBLE_COHERENT_STORAGE,
			size);

		std::memcpy(__pDataBuffer->getData(), pData, size);

		__dataBufferUpdated = true;
		_invalidate();

		__needRedrawEvent.invoke(this);
	}

	void Layer::addRenderObject(
		RenderObject const *const pObject)
	{
		if (!(__objects.emplace(pObject).second))
			throw std::runtime_error{ "The object is already added." };

		pObject->getRendererChangeEvent() += __pObjectRendererChangeListener;

		if (pObject->getRenderer())
			__registerObject(pObject);
	}

	void Layer::removeRenderObject(
		RenderObject const *const pObject)
	{
		if (!(__objects.erase(pObject)))
			throw std::runtime_error{ "The object is not added yet." };

		pObject->getRendererChangeEvent() -= __pObjectRendererChangeListener;

		if (pObject->getRenderer())
			__unregisterObject(pObject);
	}

	bool Layer::isEmpty() const noexcept
	{
		return __objects.empty();
	}

	void Layer::draw(
		VK::CommandBuffer &cmdBuffer,
		VK::ImageView &colorAttachment,
		VK::ImageView *const pDepthStencilAttachment,
		RendererResourceManager &rendererResourceManager,
		VkRect2D const &renderArea) const
	{
		for (auto const pSubLayer : __sortedSubLayers)
		{
			if (pSubLayer->isEmpty())
				continue;

			pSubLayer->draw(
				cmdBuffer, colorAttachment, pDepthStencilAttachment,
				rendererResourceManager, renderArea, __getDescSet());
		}
	}

	void Layer::_onValidate()
	{
		for (auto const pSubLayer : __invalidatedSubLayers)
			pSubLayer->validate();

		if (__subLayerSortionInvalidated)
			__sortSubLayers();

		if (__dataBufferUpdated)
			__validateDescSet();

		__invalidatedSubLayers.clear();
		__subLayerSortionInvalidated = false;
		__dataBufferUpdated = false;
	}

	void Layer::__createDescPool()
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

	void Layer::__allocDescSets()
	{
		uint32_t const descSetCount{ static_cast<uint32_t>(__descSets.size()) };

		std::vector<VkDescriptorSetLayout> layoutHandles;

		for (uint32_t iter{ }; iter < descSetCount; ++iter)
			layoutHandles.emplace_back(__layerDescSetLayout.getHandle());

		VkDescriptorSetAllocateInfo const allocInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO },
			.descriptorPool		{ __pDescPool->getHandle() },
			.descriptorSetCount	{ descSetCount },
			.pSetLayouts		{ layoutHandles.data() }
		};

		__device.vkAllocateDescriptorSets(&allocInfo, __descSets.data());
	}

	void Layer::__registerObject(
		RenderObject const *const pObject)
	{
		auto const pSubLayer{ __getSubLayerOf(pObject->getRenderer()) };
		pSubLayer->addRenderObject(pObject);

		__object2SubLayer[pObject] = pSubLayer;
	}

	void Layer::__unregisterObject(
		RenderObject const *const pObject)
	{
		auto const pSubLayer{ __object2SubLayer.extract(pObject).mapped() };
		pSubLayer->removeRenderObject(pObject);

		if (pSubLayer->isEmpty())
		{
			auto const pRenderer{ pSubLayer->getRenderer() };
			pRenderer->getPriorityChangeEvent() -= __pRendererPriorityChangeListener;

			__invalidatedSubLayers.erase(pSubLayer);
			__subLayerMap.erase(pRenderer);
		}
	}

	SubLayer *Layer::__getSubLayerOf(
		Renderer const *const pRenderer) noexcept
	{
		auto &pRetVal{ __subLayerMap[pRenderer] };
		if (!pRetVal)
		{
			pRetVal = std::make_unique<SubLayer>(
				__device, __subLayerDescSetLayout, __deferredDeleter, __scbBuilder,
				__descUpdater, __resourcePool, __globalDescManager, pRenderer);

			pRenderer->getPriorityChangeEvent()	+= __pRendererPriorityChangeListener;
			pRetVal->getInvalidateEvent() += __pSubLayerInvalidatedListener;
			pRetVal->getNeedRedrawEvent() += __pSubLayerNeedRedrawListener;

			__subLayerSortionInvalidated = true;
			_invalidate();
		}

		return pRetVal.get();
	}

	void Layer::__sortSubLayers()
	{
		__sortedSubLayers.clear();

		for (auto const &[_, pLayer] : __subLayerMap)
			__sortedSubLayers.emplace_back(pLayer.get());

		std::sort(__sortedSubLayers.begin(), __sortedSubLayers.end(), [] (auto const lhs, auto const rhs)
		{
			return (lhs->getRenderer()->getPriority() < rhs->getRenderer()->getPriority());
		});
	}

	void Layer::__validateDescSet()
	{
		__advanceDescSet();

		VkDescriptorBufferInfo const bufferInfo
		{
			.buffer	{ __pDataBuffer->getHandle() },
			.range	{ __pDataBuffer->getSize() }
		};

		__descUpdater.addInfos(
			__getDescSet(), Constants::LAYER_DATA_BUFFER_LOCATION,
			0U, 1U, VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, &bufferInfo);
	}

	void Layer::__onObjectRendererChanged(
		RenderObject const *const pObject,
		Renderer const *const pPrev,
		Renderer const *const pCur) noexcept
	{
		if (pPrev)
			__unregisterObject(pObject);

		if (pCur)
			__registerObject(pObject);
	}

	void Layer::__onRendererPriorityChanged() noexcept
	{
		__subLayerSortionInvalidated = true;
		_invalidate();
	}

	void Layer::__onSubLayerInvalidated(
		SubLayer *const pSubLayer) noexcept
	{
		__invalidatedSubLayers.emplace(pSubLayer);
		_invalidate();
	}

	void Layer::__onSubLayerRedrawNeeded() const noexcept
	{
		__needRedrawEvent.invoke(this);
	}
}