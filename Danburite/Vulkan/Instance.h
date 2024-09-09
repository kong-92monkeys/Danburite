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

		VkResult vkEnumeratePhysicalDevices(
			uint32_t *pPhysicalDeviceCount,
			VkPhysicalDevice *pPhysicalDevices) const;

		VkResult vkEnumerateDeviceExtensionProperties(
			VkPhysicalDevice physicalDevice,
			char const *pLayerName,
			uint32_t *pPropertyCount,
			VkExtensionProperties *pProperties) const;

		void vkGetPhysicalDeviceProperties2(
			VkPhysicalDevice physicalDevice,
			VkPhysicalDeviceProperties2 *pProperties) const;

		void vkGetPhysicalDeviceFeatures2(
			VkPhysicalDevice physicalDevice,
			VkPhysicalDeviceFeatures2 *pFeatures) const;

		void vkGetPhysicalDeviceMemoryProperties2(
			VkPhysicalDevice physicalDevice,
			VkPhysicalDeviceMemoryProperties2 *pMemoryProperties) const;

		void vkGetPhysicalDeviceFormatProperties2(
			VkPhysicalDevice physicalDevice,
			VkFormat format,
			VkFormatProperties2 *pFormatProperties) const;

		void vkGetPhysicalDeviceQueueFamilyProperties2(
			VkPhysicalDevice physicalDevice,
			uint32_t *pQueueFamilyPropertyCount,
			VkQueueFamilyProperties2 *pQueueFamilyProperties) const;

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