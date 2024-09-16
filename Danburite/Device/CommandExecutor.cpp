#include "CommandExecutor.h"

namespace Dev
{
	CommandExecutor::CommandExecutor(
		VK::Device &device,
		uint32_t queueFamilyIndex)
	{
		__executionThread.run([this, &device, queueFamilyIndex]
		{
			__pCmdBufferCirculator = std::make_unique<Dev::CommandBufferCirculator>(
				device, queueFamilyIndex,
				VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY, 2U, 30U);
		}).wait();
	}

	CommandExecutor::~CommandExecutor() noexcept
	{
		__executionThread.run([this]
		{
			__pCmdBufferCirculator = nullptr;
		}).wait();
	}

	void CommandExecutor::reserve(
		Job &&job) noexcept
	{
		__jobs.emplace_back(std::move(job));
	}

	std::future<VK::CommandBuffer *> CommandExecutor::execute() noexcept
	{
		auto const pPromise{ new std::promise<VK::CommandBuffer *> };

		__executionThread.silentRun([this, pPromise, jobs{ std::move(__jobs) }]
		{
			auto &cmdBuffer{ __pCmdBufferCirculator->getNext() };

			VkCommandBufferBeginInfo const cbBeginInfo
			{
				.sType	{ VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO },
				.flags	{ VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT }
			};

			cmdBuffer.vkBeginCommandBuffer(&cbBeginInfo);

			for (auto const &job : jobs)
				job(cmdBuffer);

			cmdBuffer.vkEndCommandBuffer();

			pPromise->set_value(&cmdBuffer);
			delete pPromise;
		});

		return pPromise->get_future();
	}
}