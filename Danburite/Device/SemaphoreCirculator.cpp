#include "SemaphoreCirculator.h"

namespace Dev
{
	SemaphoreCirculator::SemaphoreCirculator(
		VK::Device &device,
		VkSemaphoreType const type,
		size_t const count)
	{
		const VkSemaphoreTypeCreateInfo typeInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO },
			.semaphoreType	{ type }
		};

		const VkSemaphoreCreateInfo createInfo
		{
			.sType	{ VkStructureType::VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO },
			.pNext	{ &typeInfo }
		};

		for (size_t iter{ }; iter < count; ++iter)
			__semaphores.emplace_back(std::make_unique<VK::Semaphore>(device, createInfo));
	}
}