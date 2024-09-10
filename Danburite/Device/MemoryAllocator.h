#pragma once

#include "../Infra/MathUtil.h"
#include "../Vulkan/PhysicalDevice.h"
#include "../Vulkan/Buffer.h"
#include "../Vulkan/Image.h"
#include "MemoryBlock.h"
#include "MemoryChunk.h"
#include <unordered_map>
#include <utility>
#include <optional>

namespace Dev
{
	class MemoryAllocator : public Infra::Unique
	{
	public:
		MemoryAllocator(
			VK::PhysicalDevice const &physicalDevice,
			VK::Device &device,
			size_t memoryBlockSize,
			VkDeviceSize uniformBufferAlignment,
			VkDeviceSize storageBufferAlignment) noexcept;

		virtual ~MemoryAllocator() noexcept override = default;

		[[nodiscard]]
		std::unique_ptr<MemoryChunk> allocateBufferMemory(
			VK::Buffer &buffer,
			VkMemoryPropertyFlags memoryProp);

		[[nodiscard]]
		std::unique_ptr<MemoryChunk> allocateImageMemory(
			VK::Image &image,
			VkMemoryPropertyFlags memoryProp);

	private:
		VK::PhysicalDevice const &__physicalDevice;
		VK::Device &__device;

		size_t const __memoryBlockSize;
		VkDeviceSize const __uniformBufferAlignment;
		VkDeviceSize const __storageBufferAlignment;

		std::unordered_map<uint32_t, std::vector<std::unique_ptr<MemoryBlock>>> __bufferMemoryBlockMap;
		std::unordered_map<uint32_t, std::vector<std::unique_ptr<MemoryBlock>>> __imageMemoryBlockMap;

		[[nodiscard]]
		constexpr VkDeviceSize __resolveBufferAlignment(
			VkDeviceSize memAlignment,
			VkBufferUsageFlags usage) const noexcept;

		[[nodiscard]]
		std::optional<uint32_t> resolveMemoryTypeIndex(
			VkMemoryRequirements const &memReq,
			VkMemoryPropertyFlags const memoryProp) const noexcept;
	};

	constexpr VkDeviceSize MemoryAllocator::__resolveBufferAlignment(
		VkDeviceSize memAlignment,
		VkBufferUsageFlags const usage) const noexcept
	{
		VkDeviceSize retVal{ memAlignment };

		if (usage & VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
			retVal = Infra::MathUtil::getLCMOf(retVal, __uniformBufferAlignment);

		if (usage & VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT)
			retVal = Infra::MathUtil::getLCMOf(retVal, __storageBufferAlignment);

		return retVal;
	}
}