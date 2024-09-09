#pragma once

#include "DeviceMemory.h"

namespace VK
{
	class Buffer : public Infra::Handle<VkBuffer>
	{
	public:
		Buffer(
			Device &device,
			VkBufferCreateInfo const &createInfo);

		virtual ~Buffer() noexcept override;

		[[nodiscard]]
		constexpr VkMemoryDedicatedRequirements const &getMemoryDedicatedRequirements() const noexcept;

		[[nodiscard]]
		constexpr VkMemoryRequirements const &getMemoryRequirements() const noexcept;

	private:
		Device &__device;

		VkMemoryDedicatedRequirements __memDedicatedReq{ };
		VkMemoryRequirements2 __memReq2{ };

		void __resolveMemoryRequirements() noexcept;

		[[nodiscard]]
		static VkBuffer __create(
			Device &device,
			VkBufferCreateInfo const &createInfo);
	};

	constexpr VkMemoryDedicatedRequirements const &Buffer::getMemoryDedicatedRequirements() const noexcept
	{
		return __memDedicatedReq;
	}

	constexpr VkMemoryRequirements const &Buffer::getMemoryRequirements() const noexcept
	{
		return __memReq2.memoryRequirements;
	}
}