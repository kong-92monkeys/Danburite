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

	void Texture::updateData(
		ImageRegionInfo const &regionInfo,
		void const *pData,
		size_t const size,
		VkPipelineStageFlags2 const beforeStageMask,
		VkAccessFlags2 const beforeAccessMask,
		VkPipelineStageFlags2 const afterStageMask,
		VkAccessFlags2 const afterAccessMask,
		VkImageLayout const afterLayout)
	{
		auto pStagingBuffer{ __resourcePool.getBuffer(ResourcePool::BufferType::STAGING, size) };
		std::memcpy(pStagingBuffer->getData(), pData, size);

		__commandExecutor.reserve([=, &dst{ *__pImage }, pSrc{ std::move(pStagingBuffer) }] (auto &cmdBuffer) mutable
		{
			VkImageSubresourceRange const subresourceRange
			{
				.aspectMask			{ regionInfo.imageSubresource.aspectMask },
				.baseMipLevel		{ regionInfo.imageSubresource.mipLevel },
				.levelCount			{ 1U },
				.baseArrayLayer		{ regionInfo.imageSubresource.baseArrayLayer },
				.layerCount			{ regionInfo.imageSubresource.layerCount }
			};

			VkImageMemoryBarrier2 const beforeMemoryBarrier
			{
				.sType					{ VkStructureType::VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 },
				.srcStageMask			{ beforeStageMask },
				.srcAccessMask			{ beforeAccessMask },
				.dstStageMask			{ VK_PIPELINE_STAGE_2_COPY_BIT },
				.dstAccessMask			{ VK_ACCESS_2_TRANSFER_WRITE_BIT },
				.oldLayout				{ VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED },
				.newLayout				{ VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL },
				.image					{ dst.getHandle() },
				.subresourceRange		{ subresourceRange }
			};

			VkDependencyInfo const beforeBarrier
			{
				.sType						{ VkStructureType::VK_STRUCTURE_TYPE_DEPENDENCY_INFO },
				.imageMemoryBarrierCount	{ 1U },
				.pImageMemoryBarriers		{ &beforeMemoryBarrier }
			};

			VkBufferImageCopy2 const region
			{
				.sType				{ VkStructureType::VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2 },
				.bufferOffset		{ 0U },
				.bufferRowLength	{ regionInfo.bufferRowLength },
				.bufferImageHeight	{ regionInfo.bufferImageHeight },
				.imageSubresource	{ regionInfo.imageSubresource },
				.imageOffset		{ regionInfo.imageOffset },
				.imageExtent		{ regionInfo.imageExtent }
			};

			VkCopyBufferToImageInfo2 const copyInfo
			{
				.sType			{ VkStructureType::VK_STRUCTURE_TYPE_COPY_BUFFER_TO_IMAGE_INFO_2 },
				.srcBuffer		{ pSrc->getHandle() },
				.dstImage		{ dst.getHandle() },
				.dstImageLayout	{ VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL },
				.regionCount	{ 1U },
				.pRegions		{ &region }
			};

			VkImageMemoryBarrier2 const afterMemoryBarrier
			{
				.sType					{ VkStructureType::VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 },
				.srcStageMask			{ VK_PIPELINE_STAGE_2_COPY_BIT },
				.srcAccessMask			{ VK_ACCESS_2_TRANSFER_WRITE_BIT },
				.dstStageMask			{ afterStageMask },
				.dstAccessMask			{ afterAccessMask },
				.oldLayout				{ VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL },
				.newLayout				{ afterLayout },
				.image					{ dst.getHandle() },
				.subresourceRange		{ subresourceRange }
			};

			VkDependencyInfo const afterBarrier
			{
				.sType						{ VkStructureType::VK_STRUCTURE_TYPE_DEPENDENCY_INFO },
				.imageMemoryBarrierCount	{ 1U },
				.pImageMemoryBarriers		{ &afterMemoryBarrier }
			};

			cmdBuffer.vkCmdPipelineBarrier2(&beforeBarrier);
			cmdBuffer.vkCmdCopyBufferToImage2(&copyInfo);
			cmdBuffer.vkCmdPipelineBarrier2(&afterBarrier);

			__resourcePool.recycleBuffer(ResourcePool::BufferType::STAGING, std::move(pSrc));
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