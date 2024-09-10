#include "PipelineLayout.h"
#include <stdexcept>

namespace VK
{
	PipelineLayout::PipelineLayout(
		Device &device,
		VkPipelineLayoutCreateInfo const &createInfo) :
		Handle		{ __create(device, createInfo) },
		__device	{ device }
	{}

	PipelineLayout::~PipelineLayout() noexcept
	{
		__device.vkDestroyPipelineLayout(getHandle(), nullptr);
	}

	VkPipelineLayout PipelineLayout::__create(
		Device &device,
		VkPipelineLayoutCreateInfo const &createInfo)
	{
		VkPipelineLayout retVal{ };
		auto const result{ device.vkCreatePipelineLayout(&createInfo, nullptr, &retVal) };

		if (result != VkResult::VK_SUCCESS)
			throw std::runtime_error{ "Cannot create a VkPipelineLayout." };

		return retVal;
	}
}