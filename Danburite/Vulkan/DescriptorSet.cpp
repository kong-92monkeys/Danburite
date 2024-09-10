#include "DescriptorSet.h"
#include <stdexcept>

namespace VK
{
	DescriptorSet::DescriptorSet(
		Device &device,
		bool const needFree,
		VkDescriptorSetAllocateInfo const &allocInfo) :
		Handle		{ __allocate(device, allocInfo) },
		__device	{ device },
		__hDescPool	{ allocInfo.descriptorPool },
		__needFree	{ needFree }
	{}

	DescriptorSet::~DescriptorSet() noexcept
	{
		if (!__needFree)
			return;

		__device.vkFreeDescriptorSets(__hDescPool, 1U, &(getHandle()));
	}

	VkDescriptorSet DescriptorSet::__allocate(
		Device &device,
		VkDescriptorSetAllocateInfo const &allocInfo)
	{
		VkDescriptorSet retVal{ };
		auto const result{ device.vkAllocateDescriptorSets(&allocInfo, &retVal) };

		if (result != VkResult::VK_SUCCESS)
			throw std::runtime_error{ "Cannot allocate a VkDescriptorSet." };

		return retVal;
	}
}