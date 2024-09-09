#include "PipelineCache.h"
#include <stdexcept>

namespace VK
{
	PipelineCache::PipelineCache(
		Device &device,
		VkPipelineCacheCreateInfo const &createInfo) :
		Handle				{ __create(device, createInfo) },
		__device			{ device }
	{}

	PipelineCache::~PipelineCache() noexcept
	{
		__device.vkDestroyPipelineCache(getHandle(), nullptr);
	}

	VkPipelineCache PipelineCache::__create(
		Device &device,
		VkPipelineCacheCreateInfo const &createInfo)
	{
		VkPipelineCache retVal{ };
		auto const result{ device.vkCreatePipelineCache(&createInfo, nullptr, &retVal) };

		if (result != VkResult::VK_SUCCESS)
			throw std::runtime_error{ "Cannot create a VkPipelineCache." };

		return retVal;
	}
}