#pragma once

#include "Device.h"

namespace VK
{
	class Swapchain : public Infra::Handle<VkSwapchainKHR>
	{
	public:
		Swapchain(
			Device &device,
			VkSwapchainCreateInfoKHR const &createInfo);

		virtual ~Swapchain() noexcept override;

		VkResult vkGetSwapchainImagesKHR(
			uint32_t *pSwapchainImageCount,
			VkImage *pSwapchainImages);

	private:
		Device &__device;

		[[nodiscard]]
		static VkSwapchainKHR __create(
			Device &device,
			VkSwapchainCreateInfoKHR const &createInfo);
	};
}