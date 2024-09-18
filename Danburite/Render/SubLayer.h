#pragma once

#include "ShaderData.h"
#include "../Infra/Stateful.h"
#include "../Device/DescriptorUpdater.h"
#include "ResourcePool.h"
#include "GlobalDescriptorManager.h"
#include "RenderObject.h"

namespace Render
{
	class SubLayer : public Infra::Unique, public Infra::Stateful<SubLayer>
	{
	public:
		SubLayer(
			VK::Device &device,
			VK::DescriptorSetLayout &descSetLayout,
			Infra::DeferredDeleter &deferredDeleter,
			Dev::DescriptorUpdater &descUpdater,
			ResourcePool &resourcePool,
			GlobalDescriptorManager &globalDescManager,
			Renderer const &renderer) noexcept;

		virtual ~SubLayer() noexcept override;

		void addRenderObject(
			RenderObject const *pObject) noexcept;

		void removeRenderObject(
			RenderObject const *pObject) noexcept;

		[[nodiscard]]
		bool isEmpty() const noexcept;

		void draw(
			VK::CommandBuffer &cmdBuffer) const;

		[[nodiscard]]
		constexpr Infra::EventView<SubLayer const *> &getNeedRedrawEvent() const noexcept;

	protected:
		virtual void _onValidate() override;

	private:
		VK::Device &__device;
		VK::DescriptorSetLayout &__descSetLayout;
		Infra::DeferredDeleter &__deferredDeleter;
		Dev::DescriptorUpdater &__descUpdater;
		ResourcePool &__resourcePool;
		GlobalDescriptorManager &__globalDescManager;
		Renderer const &__renderer;

		Infra::RegionAllocator __objectRegionAllocator{ UINT32_MAX };

		std::unordered_map<RenderObject const *, std::unique_ptr<Infra::Region>> __object2Region;
		std::unordered_map<Mesh const *, std::unordered_set<RenderObject const *>> __mesh2Objects;

		bool __instanceInfoBufferInvalidated{ };
		std::vector<InstanceInfo> __instanceInfoHostBuffer;
		std::shared_ptr<Dev::MemoryBuffer> __pInstanceInfoBuffer;

		std::shared_ptr<VK::DescriptorPool> __pDescPool;

		std::array<VkDescriptorSet, Constants::MAX_IN_FLIGHT_FRAME_COUNT + 1ULL> __descSets;
		uint32_t __descSetCursor{ };
		VkDescriptorSet __hCurDescSet{ };

		Infra::EventListenerPtr<RenderObject const *, Mesh const *, Mesh const *>
			__pObjectMeshChangeListener;

		Infra::EventListenerPtr<RenderObject const *, uint32_t, std::type_index, Material const *, Material const *>
			__pObjectMaterialChangeListener;

		Infra::EventListenerPtr<RenderObject const *, uint32_t, uint32_t>
			__pObjectInstanceCountChangeListener;

		Infra::EventListenerPtr<RenderObject const *, bool>
			__pObjectDrawableChangeListener;

		mutable Infra::Event<SubLayer const *> __needRedrawEvent;

		void __createSubLayerDescSetPool();
		void __allocSubLayerDescSets();

		void __onObjectMeshChanged(
			RenderObject const *pObject,
			Mesh const *pPrev,
			Mesh const *pCur) noexcept;

		void __onObjectMaterialChanged(
			RenderObject const *pObject,
			uint32_t instanceIndex,
			std::type_index const &type,
			Material const *pPrev,
			Material const *pCur) noexcept;

		void __onObjectInstanceCountChanged(
			RenderObject const *pObject,
			uint32_t prev,
			uint32_t cur) noexcept;

		void __onObjectDrawableChanged(
			RenderObject const *pObject,
			bool cur) noexcept;

		[[nodiscard]]
		constexpr VkDescriptorSet __getDescSet() const noexcept;
		constexpr void __advanceDescSet() noexcept;
	};

	constexpr Infra::EventView<SubLayer const *> &SubLayer::getNeedRedrawEvent() const noexcept
	{
		return __needRedrawEvent;
	}

	constexpr VkDescriptorSet SubLayer::__getDescSet() const noexcept
	{
		return __descSets[__descSetCursor];
	}

	constexpr void SubLayer::__advanceDescSet() noexcept
	{
		__descSetCursor = ((__descSetCursor + 1ULL) % __descSets.size());
	}
}