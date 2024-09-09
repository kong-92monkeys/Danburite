#pragma warning(disable: 5103)

#include "Instance.h"
#include <stdexcept>

#define LOAD_INSTANCE_PROC(funcName) (__instanceProc.##funcName = reinterpret_cast<PFN_##funcName>(__loader.vkGetInstanceProcAddr(getHandle(), #funcName)))

namespace VK
{
	Instance::Instance(
		VulkanLoader &loader,
		VkInstanceCreateInfo const &createInfo) :
		Handle		{ __create(loader, createInfo) },
		__loader	{ loader }
	{
		__loadInstanceProc();
	}

	Instance::~Instance() noexcept
	{
		__instanceProc.vkDestroyInstance(getHandle(), nullptr);
	}

	VkResult Instance::vkCreateDebugUtilsMessengerEXT(
		VkDebugUtilsMessengerCreateInfoEXT const *const pCreateInfo,
		VkAllocationCallbacks const *const pAllocator,
		VkDebugUtilsMessengerEXT *const pMessenger)
	{
		return __instanceProc.vkCreateDebugUtilsMessengerEXT(
			getHandle(), pCreateInfo, pAllocator, pMessenger);
	}

	void Instance::vkDestroyDebugUtilsMessengerEXT(
		VkDebugUtilsMessengerEXT const messenger,
		VkAllocationCallbacks const *const pAllocator)
	{
		__instanceProc.vkDestroyDebugUtilsMessengerEXT(
			getHandle(), messenger, pAllocator);
	}

	VkResult Instance::vkEnumeratePhysicalDevices(
		uint32_t *const pPhysicalDeviceCount,
		VkPhysicalDevice *const pPhysicalDevices) const
	{
		return __instanceProc.vkEnumeratePhysicalDevices(
			getHandle(), pPhysicalDeviceCount, pPhysicalDevices);
	}

	VkResult Instance::vkEnumerateDeviceExtensionProperties(
		VkPhysicalDevice const physicalDevice,
		char const *const pLayerName,
		uint32_t *const pPropertyCount,
		VkExtensionProperties *const pProperties) const
	{
		return __instanceProc.vkEnumerateDeviceExtensionProperties(
			physicalDevice, pLayerName, pPropertyCount, pProperties);
	}

	void Instance::vkGetPhysicalDeviceProperties2(
		VkPhysicalDevice const physicalDevice,
		VkPhysicalDeviceProperties2 *const pProperties) const
	{
		__instanceProc.vkGetPhysicalDeviceProperties2(
			physicalDevice, pProperties);
	}

	void Instance::vkGetPhysicalDeviceFeatures2(
		VkPhysicalDevice const physicalDevice,
		VkPhysicalDeviceFeatures2 *const pFeatures) const
	{
		__instanceProc.vkGetPhysicalDeviceFeatures2(
			physicalDevice, pFeatures);
	}

	void Instance::vkGetPhysicalDeviceMemoryProperties2(
		VkPhysicalDevice const physicalDevice,
		VkPhysicalDeviceMemoryProperties2 *const pMemoryProperties) const
	{
		__instanceProc.vkGetPhysicalDeviceMemoryProperties2(
			physicalDevice, pMemoryProperties);
	}

	void Instance::vkGetPhysicalDeviceFormatProperties2(
		VkPhysicalDevice const physicalDevice,
		VkFormat const format,
		VkFormatProperties2 *const pFormatProperties) const
	{
		__instanceProc.vkGetPhysicalDeviceFormatProperties2(
			physicalDevice, format, pFormatProperties);
	}

	void Instance::vkGetPhysicalDeviceQueueFamilyProperties2(
		VkPhysicalDevice const physicalDevice,
		uint32_t *const pQueueFamilyPropertyCount,
		VkQueueFamilyProperties2 *const pQueueFamilyProperties) const
	{
		__instanceProc.vkGetPhysicalDeviceQueueFamilyProperties2(
			physicalDevice, pQueueFamilyPropertyCount, pQueueFamilyProperties);
	}

	VkBool32 Instance::vkGetPhysicalDeviceWin32PresentationSupportKHR(
		VkPhysicalDevice const physicalDevice,
		uint32_t const queueFamilyIndex) const
	{
		return __instanceProc.vkGetPhysicalDeviceWin32PresentationSupportKHR(
			physicalDevice, queueFamilyIndex);
	}

	VkResult Instance::vkCreateDevice(
		VkPhysicalDevice const physicalDevice,
		VkDeviceCreateInfo const *const pCreateInfo,
		VkAllocationCallbacks const *const pAllocator,
		VkDevice *const pDevice)
	{
		return __instanceProc.vkCreateDevice(
			physicalDevice, pCreateInfo, pAllocator, pDevice);
	}

	PFN_vkVoidFunction Instance::vkGetDeviceProcAddr(
		VkDevice const device,
		char const *const pName)
	{
		return __instanceProc.vkGetDeviceProcAddr(
			device, pName);
	}

	void Instance::__loadInstanceProc() noexcept
	{
		// VulkanInstance
		LOAD_INSTANCE_PROC(vkDestroyInstance);
		LOAD_INSTANCE_PROC(vkCreateDebugUtilsMessengerEXT);
		LOAD_INSTANCE_PROC(vkDestroyDebugUtilsMessengerEXT);
		LOAD_INSTANCE_PROC(vkEnumeratePhysicalDevices);

		// Physical device
		LOAD_INSTANCE_PROC(vkGetPhysicalDeviceProperties2);
		LOAD_INSTANCE_PROC(vkGetPhysicalDeviceFeatures2);
		LOAD_INSTANCE_PROC(vkGetPhysicalDeviceFormatProperties2);
		LOAD_INSTANCE_PROC(vkEnumerateDeviceExtensionProperties);
		LOAD_INSTANCE_PROC(vkGetPhysicalDeviceMemoryProperties2);
		LOAD_INSTANCE_PROC(vkGetPhysicalDeviceQueueFamilyProperties2);
		LOAD_INSTANCE_PROC(vkGetPhysicalDeviceSurfaceSupportKHR);
		LOAD_INSTANCE_PROC(vkGetPhysicalDeviceWin32PresentationSupportKHR);

		// Physical device - Surface
		LOAD_INSTANCE_PROC(vkGetPhysicalDeviceSurfaceCapabilities2KHR);
		LOAD_INSTANCE_PROC(vkGetPhysicalDeviceSurfaceFormats2KHR);
		LOAD_INSTANCE_PROC(vkGetPhysicalDeviceSurfacePresentModesKHR);
		LOAD_INSTANCE_PROC(vkGetPhysicalDeviceSurfacePresentModes2EXT);

		// Surface
		LOAD_INSTANCE_PROC(vkCreateWin32SurfaceKHR);
		LOAD_INSTANCE_PROC(vkDestroySurfaceKHR);

		// Device
		LOAD_INSTANCE_PROC(vkCreateDevice);
		LOAD_INSTANCE_PROC(vkGetDeviceProcAddr);
	}

	VkInstance Instance::__create(
		VulkanLoader &loader,
		VkInstanceCreateInfo const &createInfo)
	{
		VkInstance retVal{ };
		auto const result{ loader.vkCreateInstance(&createInfo, nullptr, &retVal) };

		if (result != VkResult::VK_SUCCESS)
			throw std::runtime_error{ "Cannot create a VkInstance." };

		return retVal;
	}
}