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
		std::shared_ptr<Mesh> createMesh();

		[[nodiscard]]
		std::shared_ptr<Texture> createTexture(
			Texture::ImageCreateInfo const &imageCreateInfo,
			Texture::ImageViewCreateInfo const &imageViewCreateInfo);

		void render(
			std::unordered_set<RenderTarget *> const &renderTargets);

	private:
		Dev::Context &__context;
		VK::PhysicalDevice &__physicalDevice;

		Infra::DeferredDeleter __deferredDeleter{ Constants::MAX_IN_FLIGHT_FRAME_COUNT };

		uint32_t __queueFamilyIndex{ };

		std::unique_ptr<VK::Device> __pDevice;
		std::unique_ptr<VK::Queue> __pQueue;
		std::unique_ptr<VK::PipelineCache> __pPipelineCache;

		std::unique_ptr<Dev::MemoryAllocator> __pMemoryAllocator;
		std::unique_ptr<Dev::CommandExecutor> __pGeneralCommandExecutor;
		std::unique_ptr<Dev::DescriptorUpdater> __pDescriptorUpdater;
		std::unique_ptr<ResourcePool> __pResourcePool;
		std::unique_ptr<CommandSubmitter> __pCommandSubmitter;
		std::unique_ptr<GlobalDescriptorManager> __pGlobalDescriptorManager;

		std::vector<std::unique_ptr<VK::Fence>> __submissionFences;
		size_t __submissionFenceCursor{ };
		
		void __verifyPhysicalDeviceSupport();
		void __resolveQueueFamilyIndex();
		void __createDevice();
		void __retrieveQueue();
		void __createPipelineCache();
		void __createSubmissionFences();

		[[nodiscard]]
		VK::Fence &__getNextSubmissionFence();
	};
}