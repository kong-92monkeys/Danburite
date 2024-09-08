#pragma once

#include "../Infra/Handle.h"
#include "VulkanLoader.h"

namespace VK
{
	class Instance : public Infra::Handle<VkInstance>
	{
	public:
		explicit Instance(
			VulkanLoader &loader,
			VkInstanceCreateInfo const &createInfo);

		virtual ~Instance() noexcept override;

		VkResult vkCreateDebugUtilsMessengerEXT(
			VkDebugUtilsMessengerCreateInfoEXT const *pCreateInfo,
			VkAllocationCallbacks const *pAllocator,
			VkDebugUtilsMessengerEXT *pMessenger);

		void vkDestroyDebugUtilsMessengerEXT(
			VkDebugUtilsMessengerEXT messenger,
			VkAllocationCallbacks const *pAllocator);

	private:
		VulkanLoader &__loader;

		VK::InstanceProc __instanceProc;

		void __loadInstanceProc() noexcept;

		[[nodiscard]]
		static VkInstance __create(
			VulkanLoader &loader,
			VkInstanceCreateInfo const &createInfo);
	};
}