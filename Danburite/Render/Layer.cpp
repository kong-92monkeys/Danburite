#include "Layer.h"

namespace Render
{
	Layer::Layer(
		VK::Device &device,
		VK::DescriptorSetLayout &subLayerDescSetLayout,
		Infra::DeferredDeleter &deferredDeleter,
		Dev::DescriptorUpdater &descUpdater,
		ResourcePool &resourcePool,
		GlobalDescriptorManager &globalDescManager) noexcept :
		__device				{ device },
		__subLayerDescSetLayout	{ subLayerDescSetLayout },
		__deferredDeleter		{ deferredDeleter },
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
	}

	void Layer::setPriority(
		int const priority) noexcept
	{
		if (__priority == priority)
			return;

		int const prevPriority{ __priority };
		__priority = priority;

		__priorityChangeEvent.invoke(this, prevPriority, priority);
		__needRedrawEvent.invoke(this);
	}

	void Layer::addRenderObject(
		std::shared_ptr<RenderObject const> const &pObject)
	{
		if (!(__objectRefs.emplace(pObject).second))
			throw std::runtime_error{ "The object is already added." };

		pObject->getRendererChangeEvent() += __pObjectRendererChangeListener;

		if (pObject->getRenderer())
			__registerObject(pObject.get());
	}

	void Layer::removeRenderObject(
		std::shared_ptr<RenderObject const> const &pObject)
	{
		if (!(__objectRefs.erase(pObject)))
			throw std::runtime_error{ "The object is not added yet." };

		pObject->getRendererChangeEvent() -= __pObjectRendererChangeListener;

		if (pObject->getRenderer())
			__unregisterObject(pObject.get());
	}

	bool Layer::isEmpty() const noexcept
	{
		return __objectRefs.empty();
	}

	void Layer::draw(
		VK::CommandBuffer &cmdBuffer,
		VK::ImageView &outputAttachment,
		RendererResourceManager &rendererResourceManager,
		VkRect2D const &renderArea) const
	{
		for (auto const pSubLayer : __sortedSubLayers)
		{
			if (pSubLayer->isEmpty())
				continue;

			pSubLayer->draw(
				cmdBuffer, outputAttachment,
				rendererResourceManager, renderArea);
		}
	}

	void Layer::_onValidate()
	{
		for (auto const pSubLayer : __invalidatedSubLayers)
			pSubLayer->validate();

		if (__subLayerSortionInvalidated)
			__sortSubLayers();

		__invalidatedSubLayers.clear();
		__subLayerSortionInvalidated = false;
	}

	void Layer::__registerObject(
		RenderObject const *const pObject)
	{
		auto const pSubLayer{ __getSubLayerOf(pObject->getRenderer().get()) };
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
				__device, __subLayerDescSetLayout, __deferredDeleter,
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