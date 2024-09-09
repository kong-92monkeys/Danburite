#pragma once

#include "../Vulkan/Context.h"
#include "../Vulkan/Device.h"

namespace Render
{
	class Engine : public Infra::Unique
	{
	public:
		Engine(
			VK::Context &context,
			VK::PhysicalDevice &physicalDevice);

		virtual ~Engine() noexcept override;

	private:
		VK::Context &__context;
		VK::PhysicalDevice &__physicalDevice;

		uint32_t __queueFamilyIndex{ };

		std::unique_ptr<VK::Device> __pDevice;

		void __resolveQueueFamilyIndex();
		void __createDevice();
	};
}