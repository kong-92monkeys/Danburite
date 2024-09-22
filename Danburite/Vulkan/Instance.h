#pragma once

#include "../Infra/Handle.h"
#include "VulkanLoader.h"

namespace VK
{
	class Instance : public Infra::Handle<VkInstance>
	{
	public:
		explicit Instance(
			VulkanLoader const &loader,
			VkInstanceCreateInfo const &createInfo);

		virtual ~Instance() noexcept override;

		VkResult vkCreateDebugUtilsMessengerEXT(
			VkDebugUtilsMessengerCreateInfoEXT const *pCreateInfo,
			VkAllocationCallbacks const *pAllocator,
			VkDebugUtilsMessengerEXT *pMessenger) const;

		void vkDestroyDebugUtilsMessengerEXT(
			VkDebugUtilsMessengerEXT messenger,
			VkAllocationCallbacks const *pAllocator) const;

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

		[[nodiscard]]
		VkBool32 vkGetPhysicalDeviceWin32PresentationSupportKHR(
			VkPhysicalDevice physicalDevice,
			uint32_t queueFamilyIndex) const;

		VkResult vkCreateWin32SurfaceKHR(
			VkWin32SurfaceCreateInfoKHR const *pCreateInfo,
			VkAllocationCallbacks const *pAllocator,
			VkSurfaceKHR *pSurface) const;

		void vkDestroySurfaceKHR(
			VkSurfaceKHR surface,
			VkAllocationCallbacks const *pAllocator) const;

		VkResult vkGetPhysicalDeviceSurfaceSupportKHR(
			VkPhysicalDevice physicalDevice,
			uint32_t queueFamilyIndex,
			VkSurfaceKHR surface,
			VkBool32 *pSupported) const;

		VkResult vkGetPhysicalDeviceSurfacePresentModesKHR(
			VkPhysicalDevice physicalDevice,
			VkSurfaceKHR surface,
			uint32_t *pPresentModeCount,
			VkPresentModeKHR *pPresentModes) const;

		VkResult vkGetPhysicalDeviceSurfaceCapabilities2KHR(
			VkPhysicalDevice physicalDevice,
			VkPhysicalDeviceSurfaceInfo2KHR const *pSurfaceInfo,
			VkSurfaceCapabilities2KHR *pSurfaceCapabilities) const;

		VkResult vkGetPhysicalDeviceSurfaceFormats2KHR(
			VkPhysicalDevice physicalDevice,
			VkPhysicalDeviceSurfaceInfo2KHR const *pSurfaceInfo,
			uint32_t *pSurfaceFormatCount,
			VkSurfaceFormat2KHR *pSurfaceFormats) const;

		VkResult vkCreateDevice(
			VkPhysicalDevice physicalDevice,
			VkDeviceCreateInfo const *pCreateInfo,
			VkAllocationCallbacks const *pAllocator,
			VkDevice *pDevice) const;

		[[nodiscard]]
		PFN_vkVoidFunction vkGetDeviceProcAddr(
			VkDevice device,
			char const *pName) const;

	private:
		VulkanLoader const &__loader;

		VK::InstanceProc __instanceProc;

		void __loadInstanceProc() noexcept;

		[[nodiscard]]
		static VkInstance __create(
			VulkanLoader const &loader,
			VkInstanceCreateInfo const &createInfo);
	};
}