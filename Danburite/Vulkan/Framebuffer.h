#pragma once

#include "Device.h"

namespace VK
{
	class Framebuffer : public Infra::Handle<VkFramebuffer>
	{
	public:
		Framebuffer(
			Device &device,
			VkFramebufferCreateInfo const &createInfo);

		virtual ~Framebuffer() noexcept override;

	private:
		Device &__device;

		[[nodiscard]]
		static VkFramebuffer __create(
			Device &device,
			VkFramebufferCreateInfo const &createInfo);
	};
}