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

		[[nodiscard]]
		std::byte *getData() noexcept;

		[[nodiscard]]
		constexpr VkDeviceSize getSize() const noexcept;

	private:
		std::unique_ptr<VK::Buffer> __pBuffer;
		std::unique_ptr<MemoryChunk> __pMemory;
	};

	constexpr VkDeviceSize MemoryBuffer::getSize() const noexcept
	{
		return __pBuffer->getSize();
	}
}