#pragma once

#include "../Vulkan/Fence.h"
#include "../Device/CommandExecutor.h"
#include "RenderTarget.h"

namespace Render
{
	class CommandSubmitter : public Infra::Unique
	{
	public:
		CommandSubmitter(VK::Queue &queue) noexcept;

		void addCommandExecutionResult(Dev::CommandExecutor::ExecutionResult &&result);
		void addRenderTargetDrawResult(RenderTarget::DrawResult &&result);

		void submit(VK::Fence &fence);
		void present();

		void clear();

	private:
		VK::Queue &__que;

		std::vector<Dev::CommandExecutor::ExecutionResult> __commandExecutionResults;
		std::vector<RenderTarget::DrawResult> __renderTargetDrawResults;

		std::vector<VkCommandBufferSubmitInfo> __commandBufferInfos;
		std::vector<VkSemaphoreSubmitInfo> __waitSemaphoreInfos;
		std::vector<VkSemaphoreSubmitInfo> __signalSemaphoreInfos;

		std::vector<VkSubmitInfo2> __submitInfos;
	};
}
