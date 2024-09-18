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
			VK::PhysicalDevice &physicalDevice,
			GlobalDescriptorManager::BindingInfo const &globalDescBindingInfo);

		virtual ~Engine() noexcept override;

		[[nodiscard]]
		std::unique_ptr<RenderTarget> createRenderTarget(
			HINSTANCE hinstance,
			HWND hwnd);

		[[nodiscard]]
		std::shared_ptr<Layer> createLayer();

		[[nodiscard]]
		std::shared_ptr<Mesh> createMesh();

		template <std::derived_from<Renderer> $Renderer, typename ...$Args>
		[[nodiscard]]
		std::shared_ptr<$Renderer> createRenderer($Args &&...args);

		[[nodiscard]]
		std::shared_ptr<Texture> createTexture(
			Texture::ImageCreateInfo const &imageCreateInfo,
			Texture::ImageViewCreateInfo const &imageViewCreateInfo);

		void reserveRender(
			RenderTarget *pRenderTarget) noexcept;

		void render();

	private:
		Dev::Context &__context;
		VK::PhysicalDevice &__physicalDevice;

		Infra::DeferredDeleter __deferredDeleter{ Constants::MAX_IN_FLIGHT_FRAME_COUNT };
		Dev::CommandExecutor __commandExecutor;

		uint32_t __queueFamilyIndex{ };

		std::unique_ptr<VK::Device> __pDevice;
		std::unique_ptr<VK::Queue> __pQueue;
		std::unique_ptr<VK::PipelineCache> __pPipelineCache;

		std::unique_ptr<Dev::MemoryAllocator> __pMemoryAllocator;
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

	template <std::derived_from<Renderer> $Renderer, typename ...$Args>
	std::shared_ptr<$Renderer> Engine::createRenderer($Args &&...args)
	{
		auto pRetVal{ std::make_shared<$Renderer>(std::forward<$Args>(args)...) };
		pRetVal->init(
			__physicalDevice, *__pDevice,
			*__pPipelineCache, __deferredDeleter,
			__pGlobalDescriptorManager->getDescSetLayout());

		return pRetVal;
	}
}