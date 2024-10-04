#include "Texture.h"

namespace Render
{
	Texture::Texture(
		VK::Device &device,
		Dev::CommandExecutor &commandExecutor,
		Dev::MemoryAllocator &memoryAllocator,
		Infra::DeferredDeleter &deferredDeleter,
		ResourcePool &resourcePool,
		ImageCreateInfo const &imageCreateInfo,
		ImageViewCreateInfo const &imageViewCreateInfo) :
		__device			{ device },
		__commandExecutor	{ commandExecutor },
		__memoryAllocator	{ memoryAllocator },
		__deferredDeleter	{ deferredDeleter },
		__resourcePool		{ resourcePool }
	{
		__createImage(imageCreateInfo);
		__createImageView(imageViewCreateInfo);
	}

	Texture::~Texture() noexcept
	{
		__deferredDeleter.reserve(std::move(__pImageView));
		__deferredDeleter.reserve(std::move(__pImage));
	}

	VK::ImageView const &Texture::getImageView() const noexcept
	{
		return *__pImageView;
	}

	void Texture::transitLayout(
		VkPipelineStageFlags2 const srcStageMask,
		VkAccessFlags2 const srcAccessMask,
		VkPipelineStageFlags2 const dstStageMask,
		VkAccessFlags2 const dstAccessMask,
		VkImageLayout const oldLayout,
		VkImageLayout const newLayout,
		VkImageSubresourceRange const &subresourceRange)
	{
		__commandExecutor.reserve([=, &image{ *__pImage }] (auto &cmdBuffer)
		{
			VkImageMemoryBarrier2 const imageBarrier
			{
				.sType					{ VkStructureType::VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 },
				.srcStageMask			{ srcStageMask },
				.srcAccessMask			{ srcAccessMask },
				.dstStageMask			{ dstStageMask },
				.dstAccessMask			{ dstAccessMask },
				.oldLayout				{ oldLayout },
				.newLayout				{ newLayout },
				.image					{ image.getHandle() },
				.subresourceRange		{ subresourceRange }
			};

			VkDependencyInfo const barrier
			{
				.sType						{ VkStructureType::VK_STRUCTURE_TYPE_DEPENDENCY_INFO },
				.imageMemoryBarrierCount	{ 1U },
				.pImageMemoryBarriers		{ &imageBarrier }
			};

			cmdBuffer.vkCmdPipelineBarrier2(&barrier);
		});
	}

	void Texture::updateData(
		VkBufferImageCopy2 const &region,
		VkImageLayout const imageLayout,
		void const *pData,
		size_t size)
	{
		auto pStagingBuffer{ __resourcePool.getBuffer(ResourcePool::BufferType::STAGING, size) };
		std::memcpy(pStagingBuffer->getData(), pData, size);

		__commandExecutor.reserve([=, pSrc{ std::move(pStagingBuffer) }, &dst{ *__pImage }] (auto &cmdBuffer) mutable
		{
			VkCopyBufferToImageInfo2 const copyInfo
			{
				.sType			{ VkStructureType::VK_STRUCTURE_TYPE_COPY_BUFFER_TO_IMAGE_INFO_2 },
				.srcBuffer		{ pSrc->getHandle() },
				.dstImage		{ dst.getHandle() },
				.dstImageLayout	{ VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL },
				.regionCount	{ 1U },
				.pRegions		{ &region }
			};

			cmdBuffer.vkCmdCopyBufferToImage2(&copyInfo);
			__resourcePool.recycleBuffer(ResourcePool::BufferType::STAGING, std::move(pSrc));
		});
	}

	void Texture::blit(
		VkImageLayout const srcImageLayout,
		VkImageLayout const dstImageLayout,
		VkImageBlit const &region,
		VkFilter const filter)
	{
		__commandExecutor.reserve([=, &image{ *__pImage }] (auto &cmdBuffer)
		{
			cmdBuffer.vkCmdBlitImage(
				image.getHandle(), srcImageLayout,
				image.getHandle(), dstImageLayout,
				1U, &region, filter);
		});
	}

	void Texture::__createImage(
		ImageCreateInfo const &createInfo)
	{
		VkImageCreateInfo const vkCreateInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO },
			.imageType		{ createInfo.imageType },
			.format			{ createInfo.format },
			.extent			{ createInfo.extent },
			.mipLevels		{ createInfo.mipLevels },
			.arrayLayers	{ createInfo.arrayLayers },
			.samples		{ createInfo.samples },
			.tiling			{ createInfo.tiling },
			.usage			{ createInfo.usage },
			.sharingMode	{ VkSharingMode::VK_SHARING_MODE_EXCLUSIVE }
		};

		__pImage = std::make_shared<Dev::MemoryImage>(
			__device, __memoryAllocator, vkCreateInfo,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	}

	void Texture::__createImageView(
		ImageViewCreateInfo const &createInfo)
	{
		VkImageViewCreateInfo const vkCreateInfo
		{
			.sType					{ VkStructureType::VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO },
			.image					{ __pImage->getHandle() },
			.viewType				{ createInfo.viewType },
			.format					{ createInfo.format },
			.components				{ createInfo.components },
			.subresourceRange		{ createInfo.subresourceRange }
		};

		__pImageView = std::make_shared<VK::ImageView>(__device, vkCreateInfo);
	}
}