#pragma once

#include "PhysicalDevice.h"

namespace VK
{
	class Device : public Infra::Handle<VkDevice>
	{
	public:
		Device(
			PhysicalDevice &physicalDevice,
			VkDeviceCreateInfo const &createInfo);

		virtual ~Device() noexcept override;

		VkResult vkCreatePipelineCache(
			VkPipelineCacheCreateInfo const *pCreateInfo,
			VkAllocationCallbacks const *pAllocator,
			VkPipelineCache *pPipelineCache);

		void vkDestroyPipelineCache(
			VkPipelineCache pipelineCache,
			VkAllocationCallbacks const *pAllocator);

	private:
		PhysicalDevice &__physicalDevice;

		VK::DeviceProc __deviceProc;

		void __loadDeviceProc() noexcept;

		[[nodiscard]]
		static VkDevice __create(
			PhysicalDevice &physicalDevice,
			VkDeviceCreateInfo const &createInfo);
	};
}