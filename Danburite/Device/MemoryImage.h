#pragma once

#include "MemoryAllocator.h"

namespace Dev
{
	class MemoryImage : public Infra::Handle<VkImage>
	{
	public:
		MemoryImage(
			VK::Device &device,
			MemoryAllocator &memoryAllocator,
			VkImageCreateInfo const &imageCreateInfo,
			VkMemoryPropertyFlags memoryProp);

		virtual ~MemoryImage() noexcept override;

	private:
		std::unique_ptr<VK::Image> __pImage;
		std::unique_ptr<MemoryChunk> __pMemory;
	};
}