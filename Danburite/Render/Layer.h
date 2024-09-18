#pragma once

#include "SubLayer.h"

namespace Render
{
	class Layer : public Infra::Unique, public Infra::Stateful<Layer>
	{
	public:
		Layer(
			VK::Device &device,
			VK::DescriptorSetLayout &descSetLayout,
			Infra::DeferredDeleter &deferredDeleter,
			Dev::DescriptorUpdater &descUpdater,
			ResourcePool &resourcePool,
			GlobalDescriptorManager &globalDescManager) noexcept;

		[[nodiscard]]
		constexpr int getPriority() const noexcept;
		void setPriority(
			int priority) noexcept;

		void addRenderObject(
			std::shared_ptr<RenderObject const> const &pObject);

		void removeRenderObject(
			std::shared_ptr<RenderObject const> const &pObject);

		[[nodiscard]]
		bool isEmpty() const noexcept;

		void draw(
			VK::CommandBuffer &cmdBuffer,
			VkDescriptorSet hGlobalDescSet,
			VK::ImageView &outputAttachment,
			RendererResourceManager &rendererResourceManager,
			VkRect2D const &renderArea) const;

		[[nodiscard]]
		constexpr Infra::EventView<Layer const *, int, int> &
			getPriorityChangeEvent() const noexcept;

		[[nodiscard]]
		constexpr Infra::EventView<Layer const *> &
			getNeedRedrawEvent() const noexcept;

		// TODO: Handle renderer's priority
	protected:
		virtual void _onValidate() override;

	private:
		VK::Device &__device;
		VK::DescriptorSetLayout &__descSetLayout;
		Infra::DeferredDeleter &__deferredDeleter;
		Dev::DescriptorUpdater &__descUpdater;
		ResourcePool &__resourcePool;
		GlobalDescriptorManager &__globalDescManager;

		std::unordered_set<std::shared_ptr<RenderObject const>> __objectRefs;
		std::unordered_map<Renderer const *, std::unique_ptr<SubLayer>> __subLayerMap;
		std::unordered_map<RenderObject const *, SubLayer *> __object2SubLayer;

		std::unordered_set<SubLayer *> __invalidatedSubLayers;

		bool __subLayerSortionInvalidated{ };
		std::vector<SubLayer const *> __sortedSubLayers;

		int __priority{ };

		Infra::EventListenerPtr<RenderObject const *, Renderer const *, Renderer const *>
			__pObjectRendererChangeListener;

		Infra::EventListenerPtr<Renderer const *, int, int>
			__pRendererPriorityChangeListener;

		Infra::EventListenerPtr<SubLayer *>
			__pSubLayerInvalidatedListener;

		Infra::EventListenerPtr<SubLayer const *>
			__pSubLayerNeedRedrawListener;

		mutable Infra::Event<Layer const *, int, int>
			__priorityChangeEvent;

		mutable Infra::Event<Layer const *>
			__needRedrawEvent;

		void __registerObject(
			RenderObject const *pObject);

		void __unregisterObject(
			RenderObject const *pObject);

		[[nodiscard]]
		SubLayer *__getSubLayerOf(
			Renderer const *pRenderer) noexcept;

		void __sortSubLayers();

		void __onObjectRendererChanged(
			RenderObject const *pObject,
			Renderer const *pPrev,
			Renderer const *pCur) noexcept;

		void __onRendererPriorityChanged() noexcept;

		void __onSubLayerInvalidated(
			SubLayer *pSubLayer) noexcept;

		void __onSubLayerRedrawNeeded() const noexcept;
	};

	constexpr int Layer::getPriority() const noexcept
	{
		return __priority;
	}

	constexpr Infra::EventView<Layer const *, int, int> &Layer::getPriorityChangeEvent() const noexcept
	{
		return __priorityChangeEvent;
	}

	constexpr Infra::EventView<Layer const *> &Layer::getNeedRedrawEvent() const noexcept
	{
		return __needRedrawEvent;
	}
}