#include "DescriptorSetLayout.h"
#include <stdexcept>

namespace VK
{
	DescriptorSetLayout::DescriptorSetLayout(
		Device &device,
		VkDescriptorSetLayoutCreateInfo const &createInfo) :
		Handle		{ __create(device, createInfo) },
		__device	{ device }
	{}

	DescriptorSetLayout::~DescriptorSetLayout() noexcept
	{
		__device.vkDestroyDescriptorSetLayout(getHandle(), nullptr);
	}

	VkDescriptorSetLayout DescriptorSetLayout::__create(
		Device &device,
		VkDescriptorSetLayoutCreateInfo const &createInfo)
	{
		VkDescriptorSetLayout retVal{ };
		auto const result{ device.vkCreateDescriptorSetLayout(&createInfo, nullptr, &retVal) };

		if (result != VkResult::VK_SUCCESS)
			throw std::runtime_error{ "Cannot create a VkDescriptorSetLayout." };

		return retVal;
	}
}