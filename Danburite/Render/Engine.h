#pragma once

#include "../Device/Context.h"
#include "../Vulkan/PipelineCache.h"

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

		uint32_t __queueFamilyIndex{ };

		std::unique_ptr<VK::Device> __pDevice;
		std::unique_ptr<VK::PipelineCache> __pPipelineCache;

		void __resolveQueueFamilyIndex();
		void __createDevice();
		void __createPipelineCache();
	};
}