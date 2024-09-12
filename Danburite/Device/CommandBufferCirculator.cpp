#include "CommandBufferCirculator.h"

namespace Dev
{
	CommandBufferCirculator::CommandBufferCirculator(
		VK::Device &device,
		uint32_t queueFamilyIndex,
		VkCommandBufferLevel const level,
		uint32_t const poolCount,
		uint32_t const bufferCount) :
		__poolCount		{ poolCount },
		__bufferCount	{ bufferCount }
	{
		for (uint32_t poolIter{ }; poolIter < __poolCount; ++poolIter)
			__poolResources.emplace_back(device, queueFamilyIndex, level, __bufferCount);
	}

	VK::CommandBuffer &CommandBufferCirculator::getNext()
	{
		__advanceIndex();
		return __poolResources[__poolIdx].getCommandBufferOf(__bufferIdx);
	}

	void CommandBufferCirculator::__advanceIndex()
	{
		++__bufferIdx;
		if (__bufferIdx < __bufferCount)
			return;

		__bufferIdx = 0U;
		__poolIdx = ((__poolIdx + 1U) % __poolCount);
		__poolResources[__poolIdx].resetPool(0U);
	}

	CommandBufferCirculator::PoolResource::PoolResource(
		VK::Device &device,
		uint32_t const queueFamilyIndex,
		VkCommandBufferLevel const level,
		uint32_t const bufferCount)
	{
		VkCommandPoolCreateInfo const poolCreateInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO },
			.flags				{ VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_TRANSIENT_BIT },
			.queueFamilyIndex	{ queueFamilyIndex }
		};

		__pPool = std::make_unique<VK::CommandPool>(device, poolCreateInfo);

		VkCommandBufferAllocateInfo const bufferAllocInfo
		{
			.sType					{ VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO },
			.commandPool			{ __pPool->getHandle() },
			.level					{ level },
			.commandBufferCount		{ bufferCount }
		};

		std::vector<VkCommandBuffer> bufferHandles;
		bufferHandles.resize(bufferCount);

		device.vkAllocateCommandBuffers(&bufferAllocInfo, bufferHandles.data());

		for (auto const hBuffer : bufferHandles)
			__commandBuffers.emplace_back(std::make_unique<VK::CommandBuffer>(device, hBuffer));
	}

	VkResult CommandBufferCirculator::PoolResource::resetPool(
		VkCommandPoolResetFlags const flags)
	{
		return __pPool->vkResetCommandPool(flags);
	}
}