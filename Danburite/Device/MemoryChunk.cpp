#include "MemoryChunk.h"

namespace Dev
{
	MemoryChunk_Dedicated::MemoryChunk_Dedicated(
		std::unique_ptr<VK::DeviceMemory> &&pMemory) noexcept :
		__pMemory	{ std::move(pMemory) }
	{}

	VK::DeviceMemory &MemoryChunk_Dedicated::getMemory() noexcept
	{
		return *__pMemory;
	}

	size_t MemoryChunk_Dedicated::getSize() const noexcept
	{
		return __pMemory->getSize();
	}

	size_t MemoryChunk_Dedicated::getOffset() const noexcept
	{
		return 0ULL;
	}

	MemoryChunk_SubAllocated::MemoryChunk_SubAllocated(
		VK::DeviceMemory &memory,
		std::unique_ptr<Infra::Region> &&pRegion) :
		__memory	{ memory },
		__pRegion	{ std::move(pRegion) }
	{}

	VK::DeviceMemory &MemoryChunk_SubAllocated::getMemory() noexcept
	{
		return __memory;
	}

	size_t MemoryChunk_SubAllocated::getSize() const noexcept
	{
		return __pRegion->getSize();
	}

	size_t MemoryChunk_SubAllocated::getOffset() const noexcept
	{
		return __pRegion->getOffset();
	}
}