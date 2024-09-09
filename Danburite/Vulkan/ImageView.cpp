#include "ImageView.h"
#include <stdexcept>

namespace VK
{
	ImageView::ImageView(
		Device &device,
		VkImageViewCreateInfo const &createInfo) :
		Handle		{ __create(device, createInfo) },
		__device	{ device }
	{}

	ImageView::~ImageView() noexcept
	{
		__device.vkDestroyImageView(getHandle(), nullptr);
	}

	VkImageView ImageView::__create(
		Device &device,
		VkImageViewCreateInfo const &createInfo)
	{
		VkImageView retVal{ };
		auto const result{ device.vkCreateImageView(&createInfo, nullptr, &retVal) };

		if (result != VkResult::VK_SUCCESS)
			throw std::runtime_error{ "Cannot create a VkImageView." };

		return retVal;
	}
}