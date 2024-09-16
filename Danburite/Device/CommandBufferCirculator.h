#pragma once

#include "../Vulkan/CommandBuffer.h"
#include "../Vulkan/CommandPool.h"
#include <vector>
#include <memory>

namespace Dev
{
	class CommandBufferCirculator : public Infra::Unique
	{
	public:
		CommandBufferCirculator(
			VK::Device &device,
			uint32_t queueFamilyIndex,
			VkCommandBufferLevel level,
			uint32_t poolCount,
			uint32_t bufferCount);

		[[nodiscard]]
		VK::CommandBuffer &getNext();

	private:
		class PoolResource
		{
		public:
			PoolResource(
				VK::Device &device,
				uint32_t queueFamilyIndex,
				VkCommandBufferLevel level,
				uint32_t bufferCount);

			VkResult resetPool(
				VkCommandPoolResetFlags flags);

			[[nodiscard]]
			constexpr VK::CommandBuffer &getCommandBufferOf(
				uint32_t bufferIndex) noexcept;

		private:
			std::unique_ptr<VK::CommandPool> __pPool;
			std::vector<std::unique_ptr<VK::CommandBuffer>> __commandBuffers;
		};

		uint32_t const __poolCount;
		uint32_t const __bufferCount;

		uint32_t __poolIdx{ };
		uint32_t __bufferIdx{ };

		std::vector<PoolResource> __poolResources;

		void __advance();
	};

	constexpr VK::CommandBuffer &CommandBufferCirculator::PoolResource::getCommandBufferOf(
		uint32_t bufferIndex) noexcept
	{
		return *(__commandBuffers[bufferIndex]);
	}
}