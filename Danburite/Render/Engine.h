#pragma once

#include "../Vulkan/Queue.h"
#include "../Vulkan/PipelineCache.h"
#include "../Device/Context.h"
#include "../Device/SemaphoreCirculator.h"
#include "Constants.h"
#include "RenderTarget.h"
#include "Mesh.h"
#include "Texture.h"
#include "CommandSubmitter.h"
#include "GlobalDescriptorManager.h"
#include <unordered_set>

namespace Render
{
	class Engine : public Infra::Unique
	{
	public:
		Engine(
			Dev::Context &context,
			VK::PhysicalDevice const &physicalDevice,
			GlobalDescriptorManager::BindingInfo const &globalDescBindingInfo);

		virtual ~Engine() noexcept override;

		[[nodiscard]]
		RenderTarget *createRenderTarget(
			HINSTANCE hinstance,
			HWND hwnd,
			bool useDepthBuffer,
			bool useStencilBuffer);

		[[nodiscard]]
		Layer *createLayer();

		[[nodiscard]]
		Mesh *createMesh();

		[[nodiscard]]
		Texture *createTexture(
			Texture::ImageCreateInfo const &imageCreateInfo,
			Texture::ImageViewCreateInfo const &imageViewCreateInfo);

		template <std::derived_from<Material> $Material, typename ...$Args>
		[[nodiscard]]
		$Material *createMaterial($Args &&...args);

		template <std::derived_from<Renderer> $Renderer, typename ...$Args>
		[[nodiscard]]
		$Renderer *createRenderer($Args &&...args);

		void setGlobalData(
			void const *pData,
			size_t size);

		void addGlobalMaterial(
			Material const *pMaterial);

		void removeGlobalMaterial(
			Material const *pMaterial);

		[[nodiscard]]
		uint32_t getGlobalMaterialIdOf(
			Material const *pMaterial) const;

		void reserveRender(
			RenderTarget *pRenderTarget) noexcept;

		void cancelRender(
			RenderTarget *pRenderTarget) noexcept;

		void render();

	private:
		Dev::Context &__context;
		VK::PhysicalDevice const &__physicalDevice;

		Infra::DeferredDeleter __deferredDeleter{ Constants::DEFERRED_DELETER_QUEUE_SIZE };
		Dev::CommandExecutor __commandExecutor;
		ImageReferenceManager __imageReferenceManager;

		uint32_t __queueFamilyIndex{ };

		std::unique_ptr<VK::Device> __pDevice;
		std::unique_ptr<VK::Queue> __pQueue;
		std::unique_ptr<VK::PipelineCache> __pPipelineCache;

		std::unique_ptr<Dev::MemoryAllocator> __pMemoryAllocator;
		std::unique_ptr<Dev::SCBBuilder> __pSCBBuilder;
		std::unique_ptr<Dev::DescriptorUpdater> __pDescriptorUpdater;
		std::unique_ptr<ResourcePool> __pResourcePool;
		std::unique_ptr<CommandSubmitter> __pCommandSubmitter;
		std::unique_ptr<GlobalDescriptorManager> __pGlobalDescriptorManager;
		std::unique_ptr<Dev::CommandBufferCirculator> __pExecutorCmdBufferCirculator;

		std::vector<std::unique_ptr<VK::Fence>> __submissionFences;
		size_t __submissionFenceCursor{ };
		
		std::unique_ptr<VK::DescriptorSetLayout> __pSubLayerDescSetLayout;

		std::unordered_set<RenderTarget *> __reservedRenderTargets;

		void __verifyPhysicalDeviceSupport();
		void __resolveQueueFamilyIndex();
		void __createDevice();
		void __retrieveQueue();
		void __createPipelineCache();
		void __createSubmissionFences();
		void __createSubLayerDescLayout();

		[[nodiscard]]
		VK::CommandBuffer &__beginNextExecutorCmdBuffer();

		[[nodiscard]]
		VK::Fence &__getNextSubmissionFence();

		void __validateReservedRenderTargets();
	};

	template <std::derived_from<Material> $Material, typename ...$Args>
	$Material *Engine::createMaterial($Args &&...args)
	{
		auto const pRetVal{ new $Material{ std::forward<$Args>(args)... } };
		pRetVal->init(__imageReferenceManager);

		return pRetVal;
	}

	template <std::derived_from<Renderer> $Renderer, typename ...$Args>
	$Renderer *Engine::createRenderer($Args &&...args)
	{
		auto const pRetVal{ new $Renderer{ std::forward<$Args>(args)... } };
		pRetVal->init(
			__physicalDevice, *__pDevice,
			*__pPipelineCache, __deferredDeleter,
			*__pDescriptorUpdater,
			*__pGlobalDescriptorManager,
			*__pSubLayerDescSetLayout);

		return pRetVal;
	}
}