#pragma once

#include "Device.h"

namespace VK
{
	class Image : public Infra::Handle<VkImage>
	{
	public:
		Image(
			Device &device,
			VkImageCreateInfo const &createInfo);

		virtual ~Image() noexcept override;

	private:
		Device &__device;

		[[nodiscard]]
		static VkImage __create(
			Device &device,
			VkImageCreateInfo const &createInfo);
	};
}