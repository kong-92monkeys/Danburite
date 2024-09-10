#include "Fence.h"
#include <stdexcept>

namespace VK
{
	Fence::Fence(
		Device &device,
		VkFenceCreateInfo const &createInfo) :
		Handle		{ __create(device, createInfo) },
		__device	{ device }
	{}

	Fence::~Fence() noexcept
	{
		__device.vkDestroyFence(getHandle(), nullptr);
	}

	VkFence Fence::__create(
		Device &device,
		VkFenceCreateInfo const &createInfo)
	{
		VkFence retVal{ };
		auto const result{ device.vkCreateFence(&createInfo, nullptr, &retVal) };

		if (result != VkResult::VK_SUCCESS)
			throw std::runtime_error{ "Cannot create a VkFence." };

		return retVal;
	}
}