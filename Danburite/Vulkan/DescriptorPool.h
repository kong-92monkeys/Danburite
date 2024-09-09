#pragma once

#include "Device.h"

namespace VK
{
	class DescriptorPool : public Infra::Handle<VkDescriptorPool>
	{
	public:
		DescriptorPool(
			Device &device,
			VkDescriptorPoolCreateInfo const &createInfo);

		virtual ~DescriptorPool() noexcept override;

		VkResult vkResetDescriptorPool(
			VkDescriptorPoolResetFlags flags);

	private:
		Device &__device;

		[[nodiscard]]
		static VkDescriptorPool __create(
			Device &device,
			VkDescriptorPoolCreateInfo const &createInfo);
	};
}