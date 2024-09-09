#pragma once

#include "Device.h"

namespace VK
{
	class ImageView : public Infra::Handle<VkImageView>
	{
	public:
		ImageView(
			Device &device,
			VkImageViewCreateInfo const &createInfo);

		virtual ~ImageView() noexcept override;

	private:
		Device &__device;

		[[nodiscard]]
		static VkImageView __create(
			Device &device,
			VkImageViewCreateInfo const &createInfo);
	};
}