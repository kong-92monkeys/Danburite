#pragma once

#include "MemoryAllocator.h"

namespace Dev
{
	class MemoryBuffer : public Infra::Handle<VkBuffer>
	{
	public:
		MemoryBuffer(
			VK::Device &device,
			MemoryAllocator &memoryAllocator,
			VkBufferCreateInfo const &bufferCreateInfo,
			VkMemoryPropertyFlags memoryProp);

		virtual ~MemoryBuffer() noexcept override;

	private:
		std::unique_ptr<VK::Buffer> __pBuffer;
		std::unique_ptr<MemoryChunk> __pMemory;
	};
}