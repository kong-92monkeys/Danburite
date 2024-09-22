#pragma once

#include "../Infra/Stateful.h"
#include "../Device/SCBBuilder.h"
#include "../Device/DescriptorUpdater.h"
#include "ResourcePool.h"
#include "GlobalDescriptorManager.h"
#include "RendererResourceManager.h"
#include "RenderObject.h"
#include "ShaderData.h"

namespace Render
{
	class SubLayer : public Infra::Stateful<SubLayer>
	{
	public:
		SubLayer(
			VK::Device &device,
			VK::DescriptorSetLayout &descSetLayout,
			Infra::DeferredDeleter &deferredDeleter,
			Dev::SCBBuilder &scbBuilder,
			Dev::DescriptorUpdater &descUpdater,
			ResourcePool &resourcePool,
			GlobalDescriptorManager &globalDescManager,
			Renderer const *pRenderer) noexcept;

		virtual ~SubLayer() noexcept override;

		void addRenderObject(
			RenderObject const *pObject) noexcept;

		void removeRenderObject(
			RenderObject const *pObject) noexcept;

		[[nodiscard]]
		bool isEmpty() const noexcept;

		void draw(
			VK::CommandBuffer &cmdBuffer,
			VK::ImageView &outputAttachment,
			RendererResourceManager &rendererResourceManager,
			VkRect2D const &renderArea) const;

		[[nodiscard]]
		constexpr Renderer const *getRenderer() const noexcept;

		[[nodiscard]]
		constexpr Infra::EventView<SubLayer const *> &getNeedRedrawEvent() const noexcept;

	protected:
		virtual void _onValidate() override;

	private:
		struct __ObjectDrawInfo
		{
		public:
			Mesh const *pMesh{ };
			uint32_t baseId{ };
			RenderObject const *pObject{ };
		};

		VK::Device &__device;
		VK::DescriptorSetLayout &__descSetLayout;
		Infra::DeferredDeleter &__deferredDeleter;
		Dev::SCBBuilder &__scbBuilder;
		Dev::DescriptorUpdater &__descUpdater;
		ResourcePool &__resourcePool;
		GlobalDescriptorManager &__globalDescManager;
		Renderer const *const __pRenderer;

		Infra::RegionAllocator __objectRegionAllocator{ UINT32_MAX };

		std::unordered_map<RenderObject const *, std::unique_ptr<Infra::Region>> __object2Region;
		std::unordered_map<Mesh const *, std::unordered_set<RenderObject const *>> __mesh2Objects;

		bool __instanceInfoBufferInvalidated{ };
		std::vector<InstanceInfo> __instanceInfoHostBuffer;
		std::shared_ptr<Dev::MemoryBuffer> __pInstanceInfoBuffer;

		std::shared_ptr<VK::DescriptorPool> __pDescPool;

		std::array<VkDescriptorSet, Constants::DEFERRED_DELETER_QUEUE_SIZE> __descSets{ };
		uint32_t __descSetCursor{ };

		bool __drawSequenceInvalidated{ };
		std::vector<__ObjectDrawInfo> __drawSequence;

		Infra::EventListenerPtr<RenderObject const *, Mesh const *, Mesh const *>
			__pObjectMeshChangeListener;

		Infra::EventListenerPtr<RenderObject const *, DrawParam const *, DrawParam const *>
			__pObjectDrawParamChangeListener;

		Infra::EventListenerPtr<RenderObject const *, uint32_t, std::type_index, Material const *, Material const *>
			__pObjectMaterialChangeListener;

		Infra::EventListenerPtr<RenderObject const *, uint32_t, uint32_t>
			__pObjectInstanceCountChangeListener;

		Infra::EventListenerPtr<RenderObject const *, bool>
			__pObjectDrawableChangeListener;

		mutable Infra::Event<SubLayer const *> __needRedrawEvent;

		void __createDescPool();
		void __allocDescSets();

		void __registerObject(
			RenderObject const *pObject);

		void __unregisterObject(
			RenderObject const *pObject);

		void __registerMesh(
			RenderObject const *pObject,
			Mesh const *pMesh);

		void __unregisterMesh(
			RenderObject const *pObject,
			Mesh const *pMesh);

		void __registerMaterial(
			Material const *pMaterial);

		void __unregisterMaterial(
			Material const *pMaterial);

		void __validateInstanceInfoHostBuffer(
			RenderObject const *pObject);

		void __validateInstanceInfoHostBuffer(
			RenderObject const *const pObject,
			uint32_t instanceIndex,
			std::type_index const &materialType,
			Material const *const pMaterial);

		void __validateInstanceInfoBuffer();
		void __validateDescSet();

		void __validateDrawSequence();

		void __onObjectMeshChanged(
			RenderObject const *pObject,
			Mesh const *pPrev,
			Mesh const *pCur);

		void __onObjectDrawParamChanged(
			RenderObject const *pObject,
			DrawParam const *pPrev,
			DrawParam const *pCur);

		void __onObjectMaterialChanged(
			RenderObject const *pObject,
			uint32_t instanceIndex,
			std::type_index const &type,
			Material const *pPrev,
			Material const *pCur);

		void __onObjectInstanceCountChanged(
			RenderObject const *pObject,
			uint32_t prev,
			uint32_t cur);

		void __onObjectDrawableChanged(
			RenderObject const *pObject,
			bool cur);

		void __beginRenderPass(
			VK::CommandBuffer &cmdBuffer,
			VK::RenderPass &renderPass,
			VK::Framebuffer &framebuffer,
			VkRect2D const &renderArea) const;

		void __bindDescSets(
			VK::CommandBuffer &cmdBuffer) const;

		void __endRenderPass(
			VK::CommandBuffer &cmdBuffer) const;

		[[nodiscard]]
		std::future<VK::CommandBuffer *> __subDraw(
			VK::RenderPass const &renderPass,
			VK::Framebuffer const &framebuffer,
			VK::Pipeline const &pipeline,
			size_t sequenceBegin,
			size_t sequenceEnd) const;

		[[nodiscard]]
		constexpr VkDescriptorSet __getDescSet() const noexcept;
		constexpr void __advanceDescSet() noexcept;

		static void __beginSecondaryBuffer(
			VK::CommandBuffer &secondaryBuffer,
			VK::RenderPass const &renderPass,
			VK::Framebuffer const &framebuffer);
	};

	constexpr Renderer const *SubLayer::getRenderer() const noexcept
	{
		return __pRenderer;
	}

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