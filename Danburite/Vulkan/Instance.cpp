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
		VkDebugUtilsMessengerCreateInfoEXT const *pCreateInfo,
		VkAllocationCallbacks const *pAllocator,
		VkDebugUtilsMessengerEXT *pMessenger)
	{
		return __instanceProc.vkCreateDebugUtilsMessengerEXT(
			getHandle(), pCreateInfo, pAllocator, pMessenger);
	}

	void Instance::vkDestroyDebugUtilsMessengerEXT(
		VkDebugUtilsMessengerEXT messenger,
		VkAllocationCallbacks const *pAllocator)
	{
		__instanceProc.vkDestroyDebugUtilsMessengerEXT(
			getHandle(), messenger, pAllocator);
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