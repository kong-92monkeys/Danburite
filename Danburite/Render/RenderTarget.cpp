#include "RenderTarget.h"
#include <stdexcept>

namespace Render
{
	RenderTarget::RenderTarget(
		VK::Instance &instance,
		VK::PhysicalDevice &physicalDevice,
		VK::Device &device,
		VK::Queue &queue,
		HINSTANCE const hinstance,
		HWND const hwnd) :
		__instance			{ instance },
		__physicalDevice	{ physicalDevice },
		__device			{ device },
		__que				{ queue }
	{
		__createSurface(hinstance, hwnd);
		sync();
	}

	RenderTarget::~RenderTarget() noexcept
	{
		__que.vkQueueWaitIdle();

		__pSwapchain = nullptr;
		__pSurface = nullptr;
	}

	void RenderTarget::sync()
	{
		__que.vkQueueWaitIdle();
		__syncSurface();
		__syncSwapchain();
	}

	void RenderTarget::__createSurface(
		HINSTANCE const hinstance,
		HWND const hwnd)
	{
		const VkWin32SurfaceCreateInfoKHR createInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR },
			.hinstance		{ hinstance },
			.hwnd			{ hwnd }
		};

		__pSurface = std::make_unique<VK::Surface>(__instance, createInfo);

		__presentModeInfo.sType		= VkStructureType::VK_STRUCTURE_TYPE_SURFACE_PRESENT_MODE_EXT;

		__surfaceInfo.sType			= VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR;
		__surfaceInfo.pNext			= &__presentModeInfo;
		__surfaceInfo.surface		= __pSurface->getHandle();
	}

	void RenderTarget::__syncSurface()
	{
		__verifySurfaceSupport();
		__resolvePresentMode();
		__resolveCapabilities();
		__resolveSurfaceFormat();
		__resolveCompositeAlpha();
	}

	void RenderTarget::__syncSwapchain()
	{
		auto pOldSwapchain{ std::move(__pSwapchain) };

		if (!(isPresentable()))
			return;

		__createSwapchain(pOldSwapchain.get());
	}

	void RenderTarget::__verifySurfaceSupport()
	{
		VkBool32 supported{ };
		__physicalDevice.vkGetPhysicalDeviceSurfaceSupportKHR(
			__que.getFamilyIndex(), __pSurface->getHandle(), &supported);

		if (!supported)
			throw std::runtime_error{ "The device doesn't support current surface." };
	}

	void RenderTarget::__resolvePresentMode() noexcept
	{
		uint32_t presentModeCount{ };
		__physicalDevice.vkGetPhysicalDeviceSurfacePresentModesKHR(
			__pSurface->getHandle(), &presentModeCount, nullptr);

		std::vector<VkPresentModeKHR> supportedModes;
		supportedModes.resize(presentModeCount);

		__physicalDevice.vkGetPhysicalDeviceSurfacePresentModesKHR(
			__pSurface->getHandle(), &presentModeCount, supportedModes.data());

		__presentMode = VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR;
		for (const auto mode : supportedModes)
		{
			if (mode != VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR)
				continue;

			__presentMode = VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR;
			break;
		}

		__presentModeInfo.presentMode = __presentMode;
	}

	void RenderTarget::__resolveCapabilities()
	{
		__capabilities.sType = VkStructureType::VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_KHR;
		__capabilities.pNext = nullptr;

		auto const result
		{
			__physicalDevice.vkGetPhysicalDeviceSurfaceCapabilities2KHR(
				&__surfaceInfo, &__capabilities)
		};

		if (result != VkResult::VK_SUCCESS)
			throw std::runtime_error{ "Cannot resolve capabilities." };
	}

	void RenderTarget::__resolveSurfaceFormat()
	{
		uint32_t formatCount{ };
		__physicalDevice.vkGetPhysicalDeviceSurfaceFormats2KHR(&__surfaceInfo, &formatCount, nullptr);

		std::vector<VkSurfaceFormat2KHR> formats;
		formats.resize(formatCount);

		for (auto &format : formats)
			format.sType = VkStructureType::VK_STRUCTURE_TYPE_SURFACE_FORMAT_2_KHR;

		__physicalDevice.vkGetPhysicalDeviceSurfaceFormats2KHR(&__surfaceInfo, &formatCount, formats.data());

		__surfaceFormat.format = VkFormat::VK_FORMAT_UNDEFINED;
		for (auto const &format : formats)
		{
			if (
				(format.surfaceFormat.format == VkFormat::VK_FORMAT_B8G8R8A8_SRGB) &&
				(format.surfaceFormat.colorSpace == VkColorSpaceKHR::VK_COLOR_SPACE_SRGB_NONLINEAR_KHR))
			{
				__surfaceFormat = format.surfaceFormat;
				break;
			}
		}

		if (__surfaceFormat.format == VkFormat::VK_FORMAT_UNDEFINED)
			throw std::runtime_error{ "Cannot resolve suitable surface format." };
	}

	void RenderTarget::__resolveCompositeAlpha() noexcept
	{
		auto const &capabilities{ __capabilities.surfaceCapabilities };

		for (int iterBit{ 1 }; iterBit; iterBit <<= 1)
		{
			if (capabilities.supportedCompositeAlpha & iterBit)
			{
				__compositeAlpha = static_cast<VkCompositeAlphaFlagBitsKHR>(iterBit);
				return;
			}
		}
	}

	void RenderTarget::__createSwapchain(
		VK::Swapchain *const pOldSwapchain)
	{
		auto const &capabilities{ __capabilities.surfaceCapabilities };

		VkSwapchainCreateInfoKHR const createInfo
		{
			.sType					{ VkStructureType::VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR },
			.surface				{ __pSurface->getHandle() },
			.minImageCount			{ capabilities.minImageCount },
			.imageFormat			{ __surfaceFormat.format },
			.imageColorSpace		{ __surfaceFormat.colorSpace },
			.imageExtent			{ capabilities.currentExtent },
			.imageArrayLayers		{ 1U },
			.imageUsage				{ VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT },
			.imageSharingMode		{ VkSharingMode::VK_SHARING_MODE_EXCLUSIVE },
			.preTransform			{ capabilities.currentTransform },
			.compositeAlpha			{ __compositeAlpha },
			.presentMode			{ __presentMode },
			.clipped				{ VK_TRUE },
			.oldSwapchain			{ pOldSwapchain ? pOldSwapchain->getHandle() : VK_NULL_HANDLE }
		};

		__pSwapchain = std::make_unique<VK::Swapchain>(__device, createInfo);
	}
}