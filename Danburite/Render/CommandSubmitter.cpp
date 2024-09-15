#include "CommandSubmitter.h"

namespace Render
{
	CommandSubmitter::CommandSubmitter(VK::Queue &queue) noexcept :
		__que	{ queue }
	{}

	void CommandSubmitter::addCommandExecutionResult(Dev::CommandExecutor::ExecutionResult &&result)
	{
		__commandExecutionResults.emplace_back(std::move(result));
	}

	void CommandSubmitter::addRenderTargetDrawResult(RenderTarget::DrawResult &&result)
	{
		__renderTargetDrawResults.emplace_back(std::move(result));
	}

	void CommandSubmitter::submit(VK::Fence &fence)
	{
		__submitInfos.reserve(__commandExecutionResults.size() + __renderTargetDrawResults.size());
		__commandBufferInfos.reserve(__commandExecutionResults.size() + __renderTargetDrawResults.size());
		__waitSemaphoreInfos.reserve(__renderTargetDrawResults.size());
		__signalSemaphoreInfos.reserve(__renderTargetDrawResults.size());

		for (const auto &result : __commandExecutionResults)
		{
			auto &cmdBufferInfo				{ __commandBufferInfos.emplace_back() };
			cmdBufferInfo.sType				= VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
			cmdBufferInfo.commandBuffer		= result.pCmdBuffer->getHandle();

			auto &submitInfo					{ __submitInfos.emplace_back() };
			submitInfo.sType					= VkStructureType::VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
			submitInfo.commandBufferInfoCount	= 1U;
			submitInfo.pCommandBufferInfos		= &cmdBufferInfo;
		}

		for (const auto &result : __renderTargetDrawResults)
		{
			auto &cmdBufferInfo				{ __commandBufferInfos.emplace_back() };
			cmdBufferInfo.sType				= VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
			cmdBufferInfo.commandBuffer		= result.pCmdBuffer->getHandle();

			auto &waitSemaphoreInfo			{ __waitSemaphoreInfos.emplace_back() };
			waitSemaphoreInfo.sType			= VkStructureType::VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
			waitSemaphoreInfo.semaphore		= result.pImageAcqSemaphore->getHandle();
			waitSemaphoreInfo.stageMask		= VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

			auto &signalSemaphoreInfo			{ __signalSemaphoreInfos.emplace_back() };
			signalSemaphoreInfo.sType			= VkStructureType::VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
			signalSemaphoreInfo.semaphore		= result.pSignalSemaphore->getHandle();
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

		for (const auto &result : __commandExecutionResults)
			result.completion.wait();

		for (const auto &result : __renderTargetDrawResults)
			result.completion.wait();

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
		for (auto const &result : __renderTargetDrawResults)
		{
			VkPresentInfoKHR const presentInfo
			{
				.sType					{ VkStructureType::VK_STRUCTURE_TYPE_PRESENT_INFO_KHR },
				.waitSemaphoreCount		{ 1U },
				.pWaitSemaphores		{ &(result.pSignalSemaphore->getHandle()) },
				.swapchainCount			{ 1U },
				.pSwapchains			{ &(result.pSwapchain->getHandle()) },
				.pImageIndices			{ &(result.imageIndex) }
			};

			auto const result{ __que.vkQueuePresentKHR(&presentInfo) };
			if (result != VkResult::VK_SUCCESS)
				throw std::runtime_error{ "Failed to present the draw result." };
		}
	}

	void CommandSubmitter::clear()
	{
		__commandExecutionResults.clear();
		__renderTargetDrawResults.clear();

		__commandBufferInfos.clear();
		__waitSemaphoreInfos.clear();
		__signalSemaphoreInfos.clear();

		__submitInfos.clear();
	}
}