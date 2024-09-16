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

		void reserve(
			std::future<VK::CommandBuffer *> &&submission);

		void reserve(
			RenderTarget::DrawResult &&submission);

		[[nodiscard]]
		constexpr bool isEmpty() const noexcept;

		void submit(
			VK::Fence &fence);

		void present();

		void clear();

	private:
		VK::Queue &__que;

		std::vector<std::future<VK::CommandBuffer *>> __generalSubmissions;
		std::vector<RenderTarget::DrawResult> __drawcallSubmissions;

		std::vector<VkCommandBufferSubmitInfo> __commandBufferInfos;
		std::vector<VkSemaphoreSubmitInfo> __waitSemaphoreInfos;
		std::vector<VkSemaphoreSubmitInfo> __signalSemaphoreInfos;

		std::vector<VkSubmitInfo2> __submitInfos;
	};

	constexpr bool CommandSubmitter::isEmpty() const noexcept
	{
		return (__generalSubmissions.empty() && __drawcallSubmissions.empty());
	}
}
