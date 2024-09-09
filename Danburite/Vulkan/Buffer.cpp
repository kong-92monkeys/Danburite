#include "Buffer.h"
#include <stdexcept>

namespace VK
{
	Buffer::Buffer(
		Device &device,
		VkBufferCreateInfo const &createInfo) :
		Handle		{ __create(device, createInfo) },
		__device	{ device }
	{
		__resolveMemoryRequirements();
	}

	Buffer::~Buffer() noexcept
	{
		__device.vkDestroyBuffer(getHandle(), nullptr);
	}

	void Buffer::__resolveMemoryRequirements() noexcept
	{
		__memDedicatedReq.sType = VkStructureType::VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS;

		__memReq2.sType = VkStructureType::VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
		__memReq2.pNext = &__memDedicatedReq;

		VkBufferMemoryRequirementsInfo2 const resolveInfo
		{
			.sType	{ VkStructureType::VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2 },
			.buffer	{ getHandle() }
		};
		
		__device.vkGetBufferMemoryRequirements2(&resolveInfo, &__memReq2);
	}

	VkBuffer Buffer::__create(
		Device &device,
		VkBufferCreateInfo const &createInfo)
	{
		VkBuffer retVal{ };
		auto const result{ device.vkCreateBuffer(&createInfo, nullptr, &retVal) };

		if (result != VkResult::VK_SUCCESS)
			throw std::runtime_error{ "Cannot create a VkBuffer." };

		return retVal;
	}
}