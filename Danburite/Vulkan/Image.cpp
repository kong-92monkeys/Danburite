#include "Image.h"
#include <stdexcept>

namespace VK
{
	Image::Image(
		Device &device,
		VkImageCreateInfo const &createInfo) :
		Handle		{ __create(device, createInfo) },
		__device	{ device }
	{}

	Image::~Image() noexcept
	{
		__device.vkDestroyImage(getHandle(), nullptr);
	}

	VkImage Image::__create(
		Device &device,
		VkImageCreateInfo const &createInfo)
	{
		VkImage retVal{ };
		auto const result{ device.vkCreateImage(&createInfo, nullptr, &retVal) };

		if (result != VkResult::VK_SUCCESS)
			throw std::runtime_error{ "Cannot create a VkPipelineCache." };

		return retVal;
	}
}