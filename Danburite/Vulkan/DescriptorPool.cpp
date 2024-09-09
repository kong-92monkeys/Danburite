#include "DescriptorPool.h"
#include <stdexcept>

namespace VK
{
	DescriptorPool::DescriptorPool(
		Device &device,
		VkDescriptorPoolCreateInfo const &createInfo) :
		Handle		{ __create(device, createInfo) },
		__device	{ device }
	{}

	DescriptorPool::~DescriptorPool() noexcept
	{
		__device.vkDestroyDescriptorPool(getHandle(), nullptr);
	}

	VkResult DescriptorPool::vkResetDescriptorPool(
		VkDescriptorPoolResetFlags const flags)
	{
		return __device.vkResetDescriptorPool(getHandle(), flags);
	}

	VkDescriptorPool DescriptorPool::__create(
		Device &device,
		VkDescriptorPoolCreateInfo const &createInfo)
	{
		VkDescriptorPool retVal{ };
		auto const result{ device.vkCreateDescriptorPool(&createInfo, nullptr, &retVal) };

		if (result != VkResult::VK_SUCCESS)
			throw std::runtime_error{ "Cannot create a VkDescriptorPool." };

		return retVal;
	}
}