#pragma once

#include "../Vulkan/Queue.h"
#include "../Vulkan/PipelineCache.h"
#include "../Vulkan/DescriptorSetLayout.h"
#include "../Device/Context.h"
#include "../Device/FenceCirculator.h"
#include "../Device/SemaphoreCirculator.h"
#include "Constants.h"
#include "RenderTarget.h"
#include "Mesh.h"
#include "Texture.h"
#include "ResourcePool.h"
#include "CommandSubmitter.h"
#include <unordered_set>
#include <typeindex>

namespace Render
{
	class Engine : public Infra::Unique
	{
	public:
		Engine(
			Dev::Context &context,
			VK::PhysicalDevice &physicalDevice,
			std::unordered_map<std::type_index, uint32_t> const &materialTypeIds);

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
			RenderTarget &renderTarget);

	private:
		Dev::Context &__context;
		VK::PhysicalDevice &__physicalDevice;

		Infra::DeferredDeleter __deferredDeleter{ Constants::MAX_IN_FLIGHT_FRAME_COUNT };

		uint32_t __queueFamilyIndex{ };

		std::unique_ptr<VK::Device> __pDevice;
		std::unique_ptr<VK::Queue> __pQueue;
		std::unique_ptr<VK::PipelineCache> __pPipelineCache;
		std::unique_ptr<VK::DescriptorSetLayout> __pRenderTargetDescSetLayout;
		std::unique_ptr<Dev::MemoryAllocator> __pMemoryAllocator;
		std::unique_ptr<Dev::CommandExecutor> __pGeneralCommandExecutor;
		std::unique_ptr<Dev::FenceCirculator> __pSubmissionFenceCirculator;
		std::unique_ptr<ResourcePool> __pResourcePool;
		std::unique_ptr<CommandSubmitter> __pCommandSubmitter;

		std::unordered_set<VK::Fence *> __inFlightFences;
		
		void __verifyPhysicalDeviceSupport();
		void __resolveQueueFamilyIndex();
		void __createDevice();
		void __retrieveQueue();
		void __createPipelineCache();
		void __createRenderTargetDescSetLayout();

		[[nodiscard]]
		VK::Fence &__getNextSubmissionFence() noexcept;
	};
}