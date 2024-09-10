#include "MemoryImage.h"

namespace Dev
{
	MemoryImage::MemoryImage(
		VK::Device &device,
		MemoryAllocator &memoryAllocator,
		VkImageCreateInfo const &imageCreateInfo,
		VkMemoryPropertyFlags const memoryProp)
	{
		__pImage	= std::make_unique<VK::Image>(device, imageCreateInfo);
		__pMemory	= memoryAllocator.allocateImageMemory(*__pImage, memoryProp);

		const VkBindImageMemoryInfo bindInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO },
			.image			{ __pImage->getHandle() },
			.memory			{ __pMemory->getMemory().getHandle() },
			.memoryOffset	{ __pMemory->getOffset() }
		};

		device.vkBindImageMemory2(1U, &bindInfo);
		_setHandle(__pImage->getHandle());
	}

	MemoryImage::~MemoryImage() noexcept
	{
		__pImage = nullptr;
		__pMemory = nullptr;
	}
}