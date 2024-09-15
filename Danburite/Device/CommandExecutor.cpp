#include "CommandExecutor.h"

namespace Dev
{
	CommandExecutor::CommandExecutor(
		VK::Device &device,
		uint32_t queueFamilyIndex)
	{
		__recordThread.run([this, &device, queueFamilyIndex]
		{
			__pCmdBufferCirculator = std::make_unique<Dev::CommandBufferCirculator>(
				device, queueFamilyIndex,
				VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY, 2U, 30U);
		}).wait();
	}

	CommandExecutor::~CommandExecutor() noexcept
	{
		__recordThread.run([this]
		{
			__pCmdBufferCirculator = nullptr;
		}).wait();
	}

	void CommandExecutor::reserve(
		Job &&job) noexcept
	{
		__jobs.emplace_back(std::move(job));
	}

	CommandExecutor::ExecutionResult CommandExecutor::execute() noexcept
	{
		auto &cmdBuffer{ __pCmdBufferCirculator->getCurrent() };

		auto completion
		{
			__recordThread.run([this, &cmdBuffer, jobs{ std::move(__jobs) }]
			{
				VkCommandBufferBeginInfo const cbBeginInfo
				{
					.sType	{ VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO },
					.flags	{ VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT }
				};

				cmdBuffer.vkBeginCommandBuffer(&cbBeginInfo);

				for (auto const &job : jobs)
					job(cmdBuffer);

				cmdBuffer.vkEndCommandBuffer();

				__pCmdBufferCirculator->advance();
			})
		};
		
		ExecutionResult retVal;
		retVal.completion	= std::move(completion);
		retVal.pCmdBuffer	= &cmdBuffer;

		return retVal;
	}
}