#pragma once

#include "Device.h"

namespace VK
{
	class Queue : public Infra::Handle<VkQueue>
	{
	public:
		Queue(
			Device &device,
			uint32_t familyIndex,
			uint32_t queueIndex);

		virtual ~Queue() noexcept override = default;

		VkResult vkQueueWaitIdle();

		VkResult vkQueueSubmit2(
			uint32_t submitCount,
			VkSubmitInfo2 const *pSubmits,
			VkFence fence);

		VkResult vkQueuePresentKHR(
			VkPresentInfoKHR const *pPresentInfo);

	private:
		Device &__device;

		static VkQueue __retrieve(
			Device &device,
			uint32_t familyIndex,
			uint32_t queueIndex);
	};
}