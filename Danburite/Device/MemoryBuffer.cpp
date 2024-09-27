#include "MemoryBuffer.h"

namespace Dev
{
	MemoryBuffer::MemoryBuffer(
		VK::Device &device,
		MemoryAllocator &memoryAllocator,
		VkBufferCreateInfo const &bufferCreateInfo,
		VkMemoryPropertyFlags const memoryProp)
	{
		__pBuffer = std::make_unique<VK::Buffer>(device, bufferCreateInfo);
		__pMemory = memoryAllocator.allocateBufferMemory(*__pBuffer, memoryProp);

		VkBindBufferMemoryInfo const bindInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_BIND_BUFFER_MEMORY_INFO },
			.buffer			{ __pBuffer->getHandle() },
			.memory			{ __pMemory->getMemory().getHandle() },
			.memoryOffset	{ __pMemory->getOffset() }
		};

		device.vkBindBufferMemory2(1U, &bindInfo);
		_setHandle(__pBuffer->getHandle());
	}

	MemoryBuffer::~MemoryBuffer() noexcept
	{
		__pBuffer = nullptr;
		__pMemory = nullptr;
	}

	std::byte *MemoryBuffer::getData() noexcept
	{
		return (static_cast<std::byte *>(__pMemory->getMemory().map()) + __pMemory->getOffset());
	}
}