#pragma once

#include "Device.h"

namespace VK
{
	class Fence : public Infra::Handle<VkFence>
	{
	public:
		Fence(
			Device &device,
			VkFenceCreateInfo const &createInfo);

		virtual ~Fence() noexcept override;

	private:
		Device &__device;

		[[nodiscard]]
		static VkFence __create(
			Device &device,
			VkFenceCreateInfo const &createInfo);
	};
}