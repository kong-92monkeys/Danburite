#include "CommandExecutor.h"

namespace Dev
{
	CommandExecutor::CommandExecutor(
		VK::Device &device,
		uint32_t queueFamilyIndex)
	{
		__pCmdBufferCirculator = std::make_unique<Dev::CommandBufferCirculator>(
			device, queueFamilyIndex,
			VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY, 2U, 30U);
	}

	void CommandExecutor::reserve(
		Job &&job) noexcept
	{
		__jobs.emplace_back(std::move(job));
	}

	VK::CommandBuffer *CommandExecutor::execute() noexcept
	{
		if (__jobs.empty())
			return nullptr;

		auto &cmdBuffer{ __pCmdBufferCirculator->getNext() };

		VkCommandBufferBeginInfo const cbBeginInfo
		{
			.sType	{ VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO },
			.flags	{ VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT }
		};

		cmdBuffer.vkBeginCommandBuffer(&cbBeginInfo);

		for (auto const &job : __jobs)
			job(cmdBuffer);

		cmdBuffer.vkEndCommandBuffer();

		__jobs.clear();
		return &cmdBuffer;
	}
}