#pragma once

#include "../Vulkan/Fence.h"
#include "../Device/CommandExecutor.h"
#include "RenderTarget.h"

namespace Render
{
	class CommandSubmitter : public Infra::Unique
	{
	public:
		class PresentException : public std::exception
		{
		public:
			PresentException(
				char const *_Message,
				VkResult result) noexcept;

			[[nodiscard]]
			constexpr VkResult getResult() const noexcept;

		private:
			VkResult __result{ };
		};

		CommandSubmitter(VK::Queue &queue) noexcept;

		void addGeneralExecution(
			VK::CommandBuffer const &cmdBuffer);

		void addDrawResult(
			RenderTarget::DrawResult const &result);

		void submit(
			VK::Fence &fence);

		void present();
		void clear();

	private:
		VK::Queue &__que;

		std::vector<VK::CommandBuffer const *> __generalExecutions;
		std::vector<RenderTarget::DrawResult> __drawResults;

		std::vector<VkCommandBufferSubmitInfo> __commandBufferInfos;
		std::vector<VkSemaphoreSubmitInfo> __waitSemaphoreInfos;
		std::vector<VkSemaphoreSubmitInfo> __signalSemaphoreInfos;

		std::vector<VkSubmitInfo2> __submitInfos;
	};

	constexpr VkResult CommandSubmitter::PresentException::getResult() const noexcept
	{
		return __result;
	}
}
