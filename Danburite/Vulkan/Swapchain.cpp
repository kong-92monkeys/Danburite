#include "Swapchain.h"
#include <stdexcept>

namespace VK
{
	Swapchain::Swapchain(
		Device &device,
		VkSwapchainCreateInfoKHR const &createInfo) :
		Handle		{ __create(device, createInfo) },
		__device	{ device }
	{}

	Swapchain::~Swapchain() noexcept
	{
		__device.vkDestroySwapchainKHR(getHandle(), nullptr);
	}

	VkResult Swapchain::vkGetSwapchainImagesKHR(
		uint32_t *const pSwapchainImageCount,
		VkImage *const pSwapchainImages)
	{
		return __device.vkGetSwapchainImagesKHR(
			getHandle(), pSwapchainImageCount, pSwapchainImages);
	}

	VkSwapchainKHR Swapchain::__create(
		Device &device,
		VkSwapchainCreateInfoKHR const &createInfo)
	{
		VkSwapchainKHR retVal{ };
		auto const result{ device.vkCreateSwapchainKHR(&createInfo, nullptr, &retVal) };

		if (result != VkResult::VK_SUCCESS)
			throw std::runtime_error{ "Cannot create a VkSwapchainKHR." };

		return retVal;
	}
}