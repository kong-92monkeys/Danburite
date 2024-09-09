#pragma once

#include "Device.h"

namespace VK
{
	class CommandPool : public Infra::Handle<VkCommandPool>
	{
	public:
		CommandPool(
			Device &device,
			VkCommandPoolCreateInfo const &createInfo);

		virtual ~CommandPool() noexcept override;

		VkResult vkResetCommandPool(
			VkCommandPoolResetFlags flags);

	private:
		Device &__device;

		[[nodiscard]]
		static VkCommandPool __create(
			Device &device,
			VkCommandPoolCreateInfo const &createInfo);
	};
}