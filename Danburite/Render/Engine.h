#pragma once

#include "../Vulkan/Queue.h"
#include "../Vulkan/PipelineCache.h"
#include "../Vulkan/DescriptorSetLayout.h"
#include "../Device/Context.h"
#include "../Device/MemoryAllocator.h"
#include "../Device/CommandExecutor.h"
#include "../Device/CommandBufferCirculator.h"
#include "../Device/FenceCirculator.h"
#include "../Device/SemaphoreCirculator.h"
#include "RenderTarget.h"
#include "LayerResourcePool.h"
#include <unordered_set>

namespace Render
{
	class Engine : public Infra::Unique
	{
	public:
		Engine(
			Dev::Context &context,
			VK::PhysicalDevice &physicalDevice);

		virtual ~Engine() noexcept override;

		[[nodiscard]]
		std::unique_ptr<RenderTarget> createRenderTarget(
			HINSTANCE hinstance,
			HWND hwnd);

		void render(
			RenderTarget &renderTarget);

	private:
		Dev::Context &__context;
		VK::PhysicalDevice &__physicalDevice;

		Infra::DeferredDeleter __deferredDeleter{ 5ULL };
		Dev::CommandExecutor __commandExecutor;

		uint32_t __queueFamilyIndex{ };

		std::unique_ptr<VK::Device> __pDevice;
		std::unique_ptr<VK::Queue> __pQueue;
		std::unique_ptr<VK::PipelineCache> __pPipelineCache;
		std::unique_ptr<VK::DescriptorSetLayout> __pRenderTargetDescSetLayout;
		std::unique_ptr<Dev::MemoryAllocator> __pMemoryAllocator;

		std::unique_ptr<Dev::CommandBufferCirculator> __pPrimaryCmdBufferCirculator;
		std::unique_ptr<Dev::FenceCirculator> __pSubmissionFenceCirculator;
		std::unique_ptr<Dev::SemaphoreCirculator> __pImageAcqSemaphoreCirculator;
		std::unique_ptr<Dev::SemaphoreCirculator> __pSubmissionSemaphoreCirculator;

		std::unique_ptr<LayerResourcePool> __pLayerResourcePool;

		size_t __maxInFlightSubmissionCount{ 3U };
		std::unordered_set<VK::Fence *> __inFlightFences;
		
		void __verifyPhysicalDeviceSupport();
		void __resolveQueueFamilyIndex();
		void __createDevice();
		void __retrieveQueue();
		void __createPipelineCache();
		void __createRenderTargetDescSetLayout();

		[[nodiscard]]
		uint32_t __recordPrimaryCmdBuffer(
			VK::CommandBuffer &cmdBuffer,
			VK::Semaphore &imageAcqSemaphore,
			RenderTarget &renderTarget);

		void __submitPrimaryCmdBuffer(
			VK::CommandBuffer &cmdBuffer,
			VK::Semaphore &imageAcqSemaphore,
			VK::Semaphore &submissonSemaphore);

		[[nodiscard]]
		VK::Fence &__getNextSubmissionFence() noexcept;
	};
}