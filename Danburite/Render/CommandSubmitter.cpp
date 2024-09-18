#include "CommandSubmitter.h"

namespace Render
{
	CommandSubmitter::CommandSubmitter(
		VK::Queue &queue) noexcept :
		__que	{ queue }
	{}

	void CommandSubmitter::addGeneralExecution(
		VK::CommandBuffer const &cmdBuffer)
	{
		__generalExecutions.emplace_back(&cmdBuffer);
	}

	void CommandSubmitter::addDrawResult(
		RenderTarget::DrawResult const &result)
	{
		__drawResults.emplace_back(result);
	}

	void CommandSubmitter::submit(
		VK::Fence &fence)
	{
		__commandBufferInfos.clear();
		__waitSemaphoreInfos.clear();
		__signalSemaphoreInfos.clear();
		__submitInfos.clear();

		__commandBufferInfos.reserve(__generalExecutions.size() + __drawResults.size());
		__waitSemaphoreInfos.reserve(__drawResults.size());
		__signalSemaphoreInfos.reserve(__drawResults.size());

		if (!(__generalExecutions.empty()))
		{
			for (auto const pCmdBuffer : __generalExecutions)
			{
				auto &cmdBufferInfo					{ __commandBufferInfos.emplace_back() };
				cmdBufferInfo.sType					= VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
				cmdBufferInfo.commandBuffer			= pCmdBuffer->getHandle();
			}

			auto &submitInfo					{ __submitInfos.emplace_back() };
			submitInfo.sType					= VkStructureType::VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
			submitInfo.commandBufferInfoCount	= static_cast<uint32_t>(__generalExecutions.size());
			submitInfo.pCommandBufferInfos		= __commandBufferInfos.data();
		}
		
		for (auto const &drawResult : __drawResults)
		{
			auto &cmdBufferInfo					{ __commandBufferInfos.emplace_back() };
			cmdBufferInfo.sType					= VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
			cmdBufferInfo.commandBuffer			= drawResult.pCmdBuffer->getHandle();

			auto &waitSemaphoreInfo				{ __waitSemaphoreInfos.emplace_back() };
			waitSemaphoreInfo.sType				= VkStructureType::VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
			waitSemaphoreInfo.semaphore			= drawResult.pImageAcqSemaphore->getHandle();
			waitSemaphoreInfo.stageMask			= VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

			auto &signalSemaphoreInfo			{ __signalSemaphoreInfos.emplace_back() };
			signalSemaphoreInfo.sType			= VkStructureType::VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
			signalSemaphoreInfo.semaphore		= drawResult.pSignalSemaphore->getHandle();
			signalSemaphoreInfo.stageMask		= VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

			auto &submitInfo					{ __submitInfos.emplace_back() };
			submitInfo.sType					= VkStructureType::VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
			submitInfo.commandBufferInfoCount	= 1U;
			submitInfo.pCommandBufferInfos		= &cmdBufferInfo;
			submitInfo.waitSemaphoreInfoCount	= 1U;
			submitInfo.pWaitSemaphoreInfos		= &waitSemaphoreInfo;
			submitInfo.signalSemaphoreInfoCount	= 1U;
			submitInfo.pSignalSemaphoreInfos	= &signalSemaphoreInfo;
		}

		auto const result
		{
			__que.vkQueueSubmit2(
				static_cast<uint32_t>(__submitInfos.size()),
				__submitInfos.data(), fence.getHandle())
		};

		if (result != VkResult::VK_SUCCESS)
			throw std::runtime_error{ "Failed to submit the given command buffers." };
	}

	void CommandSubmitter::present()
	{
		for (auto const &submission : __drawResults)
		{
			VkPresentInfoKHR const presentInfo
			{
				.sType					{ VkStructureType::VK_STRUCTURE_TYPE_PRESENT_INFO_KHR },
				.waitSemaphoreCount		{ 1U },
				.pWaitSemaphores		{ &(submission.pSignalSemaphore->getHandle()) },
				.swapchainCount			{ 1U },
				.pSwapchains			{ &(submission.pSwapchain->getHandle()) },
				.pImageIndices			{ &(submission.imageIndex) }
			};

			auto const result{ __que.vkQueuePresentKHR(&presentInfo) };
			if (result != VkResult::VK_SUCCESS)
				throw std::runtime_error{ "Failed to present the draw drawResult." };
		}
	}

	void CommandSubmitter::clear()
	{
		__generalExecutions.clear();
		__drawResults.clear();
	}
}