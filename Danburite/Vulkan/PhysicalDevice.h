#pragma once

#include "Instance.h"

namespace VK
{
	struct QueueFamilyInfo
	{
	public:
		VkQueueFamilyProperties const *pProps{ };
		VkQueueFamilyGlobalPriorityPropertiesKHR const *pGlobalPriorityProps{ };
	};

	class PhysicalDevice : public Infra::Handle<VkPhysicalDevice>
	{
	public:
		struct Props
		{
		public:
			VkPhysicalDeviceProperties const *p10{ };
			VkPhysicalDeviceVulkan11Properties const *p11{ };
			VkPhysicalDeviceVulkan12Properties const *p12{ };
			VkPhysicalDeviceVulkan13Properties const *p13{ };
			VkPhysicalDeviceRobustness2PropertiesEXT const *pRobustness2{ };
			VkPhysicalDeviceNestedCommandBufferPropertiesEXT const *pNestedCommandBuffer{ };
		};

		struct Features
		{
		public:
			VkPhysicalDeviceFeatures const *p10{ };
			VkPhysicalDeviceVulkan11Features const *p11{ };
			VkPhysicalDeviceVulkan12Features const *p12{ };
			VkPhysicalDeviceVulkan13Features const *p13{ };
			VkPhysicalDeviceRobustness2FeaturesEXT const *pRobustness2{ };
			VkPhysicalDeviceNestedCommandBufferFeaturesEXT const *pNestedCommandBuffer{ };
		};

		PhysicalDevice(
			Instance &instance,
			VkPhysicalDevice handle);

		[[nodiscard]]
		VkExtensionProperties const *getExtensionOf(
			std::string_view const &name) const noexcept;

		[[nodiscard]]
		VkFormatProperties getFormatPropsOf(
			VkFormat format) const;

		[[nodiscard]]
		VkBool32 vkGetPhysicalDeviceWin32PresentationSupportKHR(
			uint32_t queueFamilyIndex) const;

		VkResult vkGetPhysicalDeviceSurfaceSupportKHR(
			uint32_t queueFamilyIndex,
			VkSurfaceKHR surface,
			VkBool32 *pSupported) const;

		VkResult vkGetPhysicalDeviceSurfacePresentModesKHR(
			VkSurfaceKHR surface,
			uint32_t *pPresentModeCount,
			VkPresentModeKHR *pPresentModes) const;

		VkResult vkGetPhysicalDeviceSurfaceCapabilities2KHR(
			VkPhysicalDeviceSurfaceInfo2KHR const *pSurfaceInfo,
			VkSurfaceCapabilities2KHR *pSurfaceCapabilities) const;

		VkResult vkGetPhysicalDeviceSurfaceFormats2KHR(
			VkPhysicalDeviceSurfaceInfo2KHR const *pSurfaceInfo,
			uint32_t *pSurfaceFormatCount,
			VkSurfaceFormat2KHR *pSurfaceFormats) const;

		VkResult vkCreateDevice(
			VkDeviceCreateInfo const *pCreateInfo,
			VkAllocationCallbacks const *pAllocator,
			VkDevice *pDevice) const;

		[[nodiscard]]
		PFN_vkVoidFunction vkGetDeviceProcAddr(
			VkDevice device,
			char const *pName) const;

		[[nodiscard]]
		constexpr Props const &getProps() const noexcept;

		[[nodiscard]]
		constexpr Features const &getFeatures() const noexcept;

		[[nodiscard]]
		constexpr VkPhysicalDeviceMemoryProperties const &getMemoryProps() const noexcept;

		[[nodiscard]]
		constexpr std::vector<QueueFamilyInfo> const &getQueueFamilyInfos() const noexcept;

	private:
		Instance &__instance;

		Props __propInterface{ };
		VkPhysicalDeviceProperties2 __props2{ };
		VkPhysicalDeviceVulkan11Properties __props11{ };
		VkPhysicalDeviceVulkan12Properties __props12{ };
		VkPhysicalDeviceVulkan13Properties __props13{ };
		VkPhysicalDeviceRobustness2PropertiesEXT __propsRobustness2{ };
		VkPhysicalDeviceNestedCommandBufferPropertiesEXT __propsNestedCommandBuffer{ };

		Features __featureInterface{ };
		VkPhysicalDeviceFeatures2 __features2{ };
		VkPhysicalDeviceVulkan11Features __features11{ };
		VkPhysicalDeviceVulkan12Features __features12{ };
		VkPhysicalDeviceVulkan13Features __features13{ };
		VkPhysicalDeviceRobustness2FeaturesEXT __featuresRobustness2{ };
		VkPhysicalDeviceNestedCommandBufferFeaturesEXT __featuresNestedCommandBuffer{ };

		std::vector<VkExtensionProperties> __extensions;
		std::unordered_map<std::string_view, VkExtensionProperties const *> __extensionMap;

		VkPhysicalDeviceMemoryProperties2 __memoryProps{ };

		std::vector<VkQueueFamilyProperties2> __queueFamilyProps;
		std::vector<VkQueueFamilyGlobalPriorityPropertiesKHR> __queueFamilyGlobalPriorityProps;
		std::vector<QueueFamilyInfo> __queueFamilyInfos;


		void __resolveExtensions() noexcept;
		void __resolveProps() noexcept;
		void __resolveFeatures() noexcept;
		void __resolveMemoryProps() noexcept;
		void __resolveQueueFamilyInfos() noexcept;
	};

	constexpr PhysicalDevice::Props const &PhysicalDevice::getProps() const noexcept
	{
		return __propInterface;
	}

	constexpr PhysicalDevice::Features const &PhysicalDevice::getFeatures() const noexcept
	{
		return __featureInterface;
	}

	constexpr VkPhysicalDeviceMemoryProperties const &PhysicalDevice::getMemoryProps() const noexcept
	{
		return __memoryProps.memoryProperties;
	}

	constexpr std::vector<QueueFamilyInfo> const &PhysicalDevice::getQueueFamilyInfos() const noexcept
	{
		return __queueFamilyInfos;
	}
}