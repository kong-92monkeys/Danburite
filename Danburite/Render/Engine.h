#pragma once

#include "../Vulkan/PipelineCache.h"
#include "../Device/Context.h"
#include "../Device/MemoryAllocator.h"
#include "LayerResourcePool.h"

namespace Render
{
	class Engine : public Infra::Unique
	{
	public:
		Engine(
			Dev::Context &context,
			VK::PhysicalDevice &physicalDevice);

		virtual ~Engine() noexcept override;

	private:
		Dev::Context &__context;
		VK::PhysicalDevice &__physicalDevice;

		Infra::LazyDeleter __lazyDeleter{ 5ULL };
		uint32_t __queueFamilyIndex{ };

		std::unique_ptr<VK::Device> __pDevice;
		std::unique_ptr<VK::PipelineCache> __pPipelineCache;
		std::unique_ptr<Dev::MemoryAllocator> __pMemoryAllocator;
		std::unique_ptr<LayerResourcePool> __pLayerResourcePool;

		void __resolveQueueFamilyIndex();
		void __createDevice();
		void __createPipelineCache();
	};
}