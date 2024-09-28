#include "PhysicalDevice.h"
#include <stdexcept>

namespace VK
{
	PhysicalDevice::PhysicalDevice(
		Instance &instance,
		VkPhysicalDevice const handle) :
		Handle		{ handle },
		__instance	{ instance }
	{
		__resolveExtensions();
		__resolveProps();
		__resolveFeatures();
		__resolveMemoryProps();
		__resolveQueueFamilyInfos();
	}

	VkExtensionProperties const *PhysicalDevice::getExtensionOf(
		std::string_view const &name) const noexcept
	{
		auto const foundIt{ __extensionMap.find(name) };
		if (foundIt == __extensionMap.end())
			return nullptr;

		return foundIt->second;
	}

	VkFormatProperties PhysicalDevice::getFormatPropsOf(
		VkFormat const format) const
	{
		VkFormatProperties2 props2
		{
			.sType{ VkStructureType::VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2 },
		};

		__instance.vkGetPhysicalDeviceFormatProperties2(getHandle(), format, &props2);
		return props2.formatProperties;
	}

	VkBool32 PhysicalDevice::vkGetPhysicalDeviceWin32PresentationSupportKHR(
		uint32_t const queueFamilyIndex) const
	{
		return __instance.vkGetPhysicalDeviceWin32PresentationSupportKHR(
			getHandle(), queueFamilyIndex);
	}

	VkResult PhysicalDevice::vkGetPhysicalDeviceSurfaceSupportKHR(
		uint32_t const queueFamilyIndex,
		VkSurfaceKHR const surface,
		VkBool32 *const pSupported) const
	{
		return __instance.vkGetPhysicalDeviceSurfaceSupportKHR(
			getHandle(), queueFamilyIndex, surface, pSupported);
	}

	VkResult PhysicalDevice::vkGetPhysicalDeviceSurfacePresentModesKHR(
		VkSurfaceKHR const surface,
		uint32_t *const pPresentModeCount,
		VkPresentModeKHR *const pPresentModes) const
	{
		return __instance.vkGetPhysicalDeviceSurfacePresentModesKHR(
			getHandle(), surface, pPresentModeCount, pPresentModes);
	}

	VkResult PhysicalDevice::vkGetPhysicalDeviceSurfaceCapabilities2KHR(
		VkPhysicalDeviceSurfaceInfo2KHR const *const pSurfaceInfo,
		VkSurfaceCapabilities2KHR *const pSurfaceCapabilities) const
	{
		return __instance.vkGetPhysicalDeviceSurfaceCapabilities2KHR(
			getHandle(), pSurfaceInfo, pSurfaceCapabilities);
	}

	VkResult PhysicalDevice::vkGetPhysicalDeviceSurfaceFormats2KHR(
		VkPhysicalDeviceSurfaceInfo2KHR const *const pSurfaceInfo,
		uint32_t *const pSurfaceFormatCount,
		VkSurfaceFormat2KHR *const pSurfaceFormats) const
	{
		return __instance.vkGetPhysicalDeviceSurfaceFormats2KHR(
			getHandle(), pSurfaceInfo, pSurfaceFormatCount, pSurfaceFormats);
	}

	VkResult PhysicalDevice::vkCreateDevice(
		VkDeviceCreateInfo const *const pCreateInfo,
		VkAllocationCallbacks const *const pAllocator,
		VkDevice *const pDevice) const
	{
		return __instance.vkCreateDevice(
			getHandle(), pCreateInfo, pAllocator, pDevice);
	}

	PFN_vkVoidFunction PhysicalDevice::vkGetDeviceProcAddr(
		VkDevice const device,
		char const *const pName) const
	{
		return __instance.vkGetDeviceProcAddr(device, pName);
	}

	void PhysicalDevice::__resolveExtensions() noexcept
	{
		uint32_t extensionCount{ };
		__instance.vkEnumerateDeviceExtensionProperties(getHandle(), nullptr, &extensionCount, nullptr);

		__extensions.resize(extensionCount);
		__instance.vkEnumerateDeviceExtensionProperties(getHandle(), nullptr, &extensionCount, __extensions.data());

		for (auto const &extension : __extensions)
			__extensionMap[extension.extensionName] = &extension;
	}

	void PhysicalDevice::__resolveProps() noexcept
	{
		__propInterface.p10						= &(__props2.properties);
		__propInterface.p11						= &__props11;
		__propInterface.p12						= &__props12;
		__propInterface.p13						= &__props13;
		__propInterface.pRobustness2			= &__propsRobustness2;

		__props2.sType							= VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
		__props2.pNext							= &__props11;

		__props11.sType							= VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES;
		__props11.pNext							= &__props12;

		__props12.sType							= VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES;
		__props12.pNext							= &__props13;

		__props13.sType							= VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES;
		__props13.pNext							= &__propsRobustness2;

		__propsRobustness2.sType				= VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_PROPERTIES_EXT;
		__propsRobustness2.pNext				= nullptr;

		__instance.vkGetPhysicalDeviceProperties2(getHandle(), &__props2);
	}

	void PhysicalDevice::__resolveFeatures() noexcept
	{
		__featureInterface.p10							= &(__features2.features);
		__featureInterface.p11							= &__features11;
		__featureInterface.p12							= &__features12;
		__featureInterface.p13							= &__features13;
		__featureInterface.pRobustness2					= &__featuresRobustness2;
		__featureInterface.pVertexInputDynamicState		= &__featuresVertexInputDynamicState;

		__features2.sType								= VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		__features2.pNext								= &__features11;

		__features11.sType								= VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
		__features11.pNext								= &__features12;

		__features12.sType								= VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
		__features12.pNext								= &__features13;

		__features13.sType								= VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
		__features13.pNext								= &__featuresRobustness2;

		__featuresRobustness2.sType						= VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT;
		__featuresRobustness2.pNext						= &__featuresVertexInputDynamicState;

		__featuresVertexInputDynamicState.sType			= VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_INPUT_DYNAMIC_STATE_FEATURES_EXT;
		__featuresVertexInputDynamicState.pNext			= nullptr;

		__instance.vkGetPhysicalDeviceFeatures2(getHandle(), &__features2);
	}

	void PhysicalDevice::__resolveMemoryProps() noexcept
	{
		__memoryProps.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
		__instance.vkGetPhysicalDeviceMemoryProperties2(getHandle(), &__memoryProps);
	}

	void PhysicalDevice::__resolveQueueFamilyInfos() noexcept
	{
		uint32_t familyCount{ };
		__instance.vkGetPhysicalDeviceQueueFamilyProperties2(getHandle(), &familyCount, nullptr);

		__queueFamilyProps.resize(familyCount);
		__queueFamilyGlobalPriorityProps.resize(familyCount);
		__queueFamilyInfos.resize(familyCount);

		for (uint32_t iter{ }; iter < familyCount; ++iter)
		{
			auto &props					{ __queueFamilyProps[iter] };
			auto &globalPriorityProps	{ __queueFamilyGlobalPriorityProps[iter] };
			auto &infos					{ __queueFamilyInfos[iter] };

			props.sType = VkStructureType::VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
			props.pNext = &globalPriorityProps;

			globalPriorityProps.sType = VkStructureType::VK_STRUCTURE_TYPE_QUEUE_FAMILY_GLOBAL_PRIORITY_PROPERTIES_KHR;
			globalPriorityProps.pNext = nullptr;

			infos.pProps = &(props.queueFamilyProperties);
			infos.pGlobalPriorityProps = &globalPriorityProps;
		}

		__instance.vkGetPhysicalDeviceQueueFamilyProperties2(getHandle(), &familyCount, __queueFamilyProps.data());
	}
}