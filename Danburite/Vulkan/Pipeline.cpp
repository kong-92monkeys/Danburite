#include "Pipeline.h"
#include <stdexcept>

namespace VK
{
	Pipeline::Pipeline(
		Device &device,
		VkPipelineCache const hPipelineCache,
		VkGraphicsPipelineCreateInfo const &createInfo) :
		Handle		{ __create(device, hPipelineCache, createInfo) },
		__device	{ device }
	{}

	Pipeline::Pipeline(
		Device &device,
		VkPipelineCache const hPipelineCache,
		VkComputePipelineCreateInfo const &createInfo) :
		Handle		{ __create(device, hPipelineCache, createInfo) },
		__device	{ device }
	{}

	Pipeline::~Pipeline() noexcept
	{
		__device.vkDestroyPipeline(getHandle(), nullptr);
	}

	VkPipeline Pipeline::__create(
		Device &device,
		VkPipelineCache const hPipelineCache,
		VkGraphicsPipelineCreateInfo const &createInfo)
	{
		VkPipeline retVal{ };
		auto const result{ device.vkCreateGraphicsPipelines(hPipelineCache , 1U, &createInfo, nullptr, &retVal) };

		if (result != VkResult::VK_SUCCESS)
			throw std::runtime_error{ "Cannot create a VkPipeline." };

		return retVal;
	}

	VkPipeline Pipeline::__create(
		Device &device,
		VkPipelineCache const hPipelineCache,
		VkComputePipelineCreateInfo const &createInfo)
	{
		VkPipeline retVal{ };
		auto const result{ device.vkCreateComputePipelines(hPipelineCache, 1U, &createInfo, nullptr, &retVal) };

		if (result != VkResult::VK_SUCCESS)
			throw std::runtime_error{ "Cannot create a VkPipeline." };

		return retVal;
	}
}