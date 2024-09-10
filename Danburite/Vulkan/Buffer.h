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
		constexpr VkDeviceSize getSize() const noexcept;

		[[nodiscard]]
		constexpr VkBufferUsageFlags getUsage() const noexcept;

		[[nodiscard]]
		constexpr VkMemoryDedicatedRequirements const &getMemoryDedicatedRequirements() const noexcept;

		[[nodiscard]]
		constexpr VkMemoryRequirements const &getMemoryRequirements() const noexcept;

	private:
		Device &__device;

		VkDeviceSize const __size;
		VkBufferUsageFlags const __usage;

		VkMemoryDedicatedRequirements __memDedicatedReq{ };
		VkMemoryRequirements2 __memReq2{ };

		void __resolveMemoryRequirements() noexcept;

		[[nodiscard]]
		static VkBuffer __create(
			Device &device,
			VkBufferCreateInfo const &createInfo);
	};

	constexpr VkDeviceSize Buffer::getSize() const noexcept
	{
		return __size;
	}

	constexpr VkBufferUsageFlags Buffer::getUsage() const noexcept
	{
		return __usage;
	}

	constexpr VkMemoryDedicatedRequirements const &Buffer::getMemoryDedicatedRequirements() const noexcept
	{
		return __memDedicatedReq;
	}

	constexpr VkMemoryRequirements const &Buffer::getMemoryRequirements() const noexcept
	{
		return __memReq2.memoryRequirements;
	}
}