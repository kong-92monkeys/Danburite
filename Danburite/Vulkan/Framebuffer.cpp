#include "Framebuffer.h"
#include <stdexcept>

namespace VK
{
	Framebuffer::Framebuffer(
		Device &device,
		VkFramebufferCreateInfo const &createInfo) :
		Handle		{ __create(device, createInfo) },
		__device	{ device }
	{}

	Framebuffer::~Framebuffer() noexcept
	{
		__device.vkDestroyFramebuffer(getHandle(), nullptr);
	}

	VkFramebuffer Framebuffer::__create(
		Device &device,
		VkFramebufferCreateInfo const &createInfo)
	{
		VkFramebuffer retVal{ };
		auto const result{ device.vkCreateFramebuffer(&createInfo, nullptr, &retVal) };

		if (result != VkResult::VK_SUCCESS)
			throw std::runtime_error{ "Cannot create a VkFramebuffer." };

		return retVal;
	}
}