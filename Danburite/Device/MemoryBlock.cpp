#include "MemoryBlock.h"

namespace Dev
{
	MemoryBlock::MemoryBlock(
		VK::Device &device,
		VkDeviceSize const size,
		uint32_t const memoryTypeIndex) :
		__regionAllocator	{ size }
	{
		VkMemoryAllocateInfo const memAllocInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO },
			.allocationSize		{ size },
			.memoryTypeIndex	{ memoryTypeIndex }
		};

		__pMemory = std::make_unique<VK::DeviceMemory>(device, memAllocInfo);
	}

	std::unique_ptr<Infra::Region> MemoryBlock::allocateRegion(
		size_t const size,
		size_t const alignment)
	{
		return std::make_unique<Infra::Region>(__regionAllocator, size, alignment);
	}
}