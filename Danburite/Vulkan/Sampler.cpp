#include "Sampler.h"
#include <stdexcept>

namespace VK
{
	Sampler::Sampler(
		Device &device,
		VkSamplerCreateInfo const &createInfo) :
		Handle		{ __create(device, createInfo) },
		__device	{ device }
	{}

	Sampler::~Sampler() noexcept
	{
		__device.vkDestroySampler(getHandle(), nullptr);
	}

	VkSampler Sampler::__create(
		Device &device,
		VkSamplerCreateInfo const &createInfo)
	{
		VkSampler retVal{ };
		auto const result{ device.vkCreateSampler(&createInfo, nullptr, &retVal) };

		if (result != VkResult::VK_SUCCESS)
			throw std::runtime_error{ "Cannot create a VkSampler." };

		return retVal;
	}
}