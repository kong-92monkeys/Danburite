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

		[[nodiscard]]
		constexpr uint32_t getFamilyIndex() const noexcept;

		[[nodiscard]]
		constexpr uint32_t getQueueIndex() const noexcept;

	private:
		Device &__device;

		uint32_t const __familyIndex;
		uint32_t const __queueIndex;

		static VkQueue __retrieve(
			Device &device,
			uint32_t familyIndex,
			uint32_t queueIndex);
	};

	constexpr uint32_t Queue::getFamilyIndex() const noexcept
	{
		return __familyIndex;
	}

	constexpr uint32_t Queue::getQueueIndex() const noexcept
	{
		return __queueIndex;
	}
}