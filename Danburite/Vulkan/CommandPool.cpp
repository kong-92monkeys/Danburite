#include "CommandPool.h"
#include <stdexcept>

namespace VK
{
	CommandPool::CommandPool(
		Device &device,
		VkCommandPoolCreateInfo const &createInfo) :
		Handle		{ __create(device, createInfo) },
		__device	{ device }
	{}

	CommandPool::~CommandPool() noexcept
	{
		__device.vkDestroyCommandPool(getHandle(), nullptr);
	}

	VkResult CommandPool::vkResetCommandPool(
		VkCommandPoolResetFlags const flags)
	{
		return __device.vkResetCommandPool(getHandle(), flags);
	}

	VkCommandPool CommandPool::__create(
		Device &device,
		VkCommandPoolCreateInfo const &createInfo)
	{
		VkCommandPool retVal{ };
		auto const result{ device.vkCreateCommandPool(&createInfo, nullptr, &retVal) };

		if (result != VkResult::VK_SUCCESS)
			throw std::runtime_error{ "Cannot create a VkCommandPool." };

		return retVal;
	}
}