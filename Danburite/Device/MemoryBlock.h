#pragma once

#include "../Infra/RegionAllocator.h"
#include "../Vulkan/Device.h"
#include "../Vulkan/DeviceMemory.h"
#include <memory>

namespace Dev
{
	class MemoryBlock : public Infra::Unique
	{
	public:
		MemoryBlock(
			VK::Device &device,
			VkDeviceSize size,
			uint32_t memoryTypeIndex);

		[[nodiscard]]
		constexpr VK::DeviceMemory &getMemory() noexcept;

		[[nodiscard]]
		std::unique_ptr<Infra::Region> allocateRegion(
			size_t size,
			size_t alignment);

	private:
		Infra::RegionAllocator __regionAllocator;
		std::unique_ptr<VK::DeviceMemory> __pMemory;
	};

	constexpr VK::DeviceMemory &MemoryBlock::getMemory() noexcept
	{
		return *__pMemory;
	}
}