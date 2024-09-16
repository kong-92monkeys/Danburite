#include "CommandSubmitter.h"

namespace Render
{
	CommandSubmitter::CommandSubmitter(
		VK::Queue &queue) noexcept :
		__que	{ queue }
	{}

	void CommandSubmitter::reserve(
		std::future<VK::CommandBuffer *> &&result)
	{
		__generalSubmissions.emplace_back(std::move(result));
	}

	void CommandSubmitter::reserve(
		RenderTarget::DrawResult &&result)
	{
		__drawcallSubmissions.emplace_back(std::move(result));
	}

	void CommandSubmitter::submit(
		VK::Fence &fence)
	{
		__submitInfos.reserve(__generalSubmissions.size() + __drawcallSubmissions.size());
		__commandBufferInfos.reserve(__generalSubmissions.size() + __drawcallSubmissions.size());
		__waitSemaphoreInfos.reserve(__drawcallSubmissions.size());
		__signalSemaphoreInfos.reserve(__drawcallSubmissions.size());

		for (auto &submission : __generalSubmissions)
		{
			auto &cmdBufferInfo					{ __commandBufferInfos.emplace_back() };
			cmdBufferInfo.sType					= VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
			cmdBufferInfo.commandBuffer			= submission.get()->getHandle();

			auto &submitInfo					{ __submitInfos.emplace_back() };
			submitInfo.sType					= VkStructureType::VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
			submitInfo.commandBufferInfoCount	= 1U;
			submitInfo.pCommandBufferInfos		= &cmdBufferInfo;
		}

		for (auto &submission : __drawcallSubmissions)
		{
			auto &cmdBufferInfo					{ __commandBufferInfos.emplace_back() };
			cmdBufferInfo.sType					= VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
			cmdBufferInfo.commandBuffer			= submission.cmdBuffer.get()->getHandle();

			auto &waitSemaphoreInfo				{ __waitSemaphoreInfos.emplace_back() };
			waitSemaphoreInfo.sType				= VkStructureType::VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
			waitSemaphoreInfo.semaphore			= submission.pImageAcqSemaphore->getHandle();
			waitSemaphoreInfo.stageMask			= VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

			auto &signalSemaphoreInfo			{ __signalSemaphoreInfos.emplace_back() };
			signalSemaphoreInfo.sType			= VkStructureType::VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
			signalSemaphoreInfo.semaphore		= submission.pSignalSemaphore->getHandle();
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
		for (auto const &submission : __drawcallSubmissions)
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
				throw std::runtime_error{ "Failed to present the draw submission." };
		}
	}

	void CommandSubmitter::clear()
	{
		__generalSubmissions.clear();
		__drawcallSubmissions.clear();

		__commandBufferInfos.clear();
		__waitSemaphoreInfos.clear();
		__signalSemaphoreInfos.clear();

		__submitInfos.clear();
	}
}