#include "FenceCirculator.h"

namespace Dev
{
	FenceCirculator::FenceCirculator(
		VK::Device &device,
		size_t const count)
	{
		VkFenceCreateInfo const createInfo
		{
			.sType	{ VkStructureType::VK_STRUCTURE_TYPE_FENCE_CREATE_INFO },
			.flags	{ VkFenceCreateFlagBits::VK_FENCE_CREATE_SIGNALED_BIT }
		};

		for (size_t iter{ }; iter < count; ++iter)
			__fences.emplace_back(std::make_unique<VK::Fence>(device, createInfo));
	}
}