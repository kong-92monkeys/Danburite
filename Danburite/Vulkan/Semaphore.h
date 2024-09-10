#pragma once

#include "Device.h"

namespace VK
{
	class Semaphore : public Infra::Handle<VkSemaphore>
	{
	public:
		Semaphore(
			Device &device,
			VkSemaphoreCreateInfo const &createInfo);

		virtual ~Semaphore() noexcept override;

	private:
		Device &__device;

		[[nodiscard]]
		static VkSemaphore __create(
			Device &device,
			VkSemaphoreCreateInfo const &createInfo);
	};
}