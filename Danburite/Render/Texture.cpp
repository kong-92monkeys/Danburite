#include "Texture.h"

namespace Render
{
	Texture::Texture(
		VK::Device &device,
		Dev::CommandExecutor &commandExecutor,
		Dev::MemoryAllocator &memoryAllocator,
		Infra::DeferredDeleter &deferredDeleter,
		ImageCreateInfo const &imageCreateInfo,
		ImageViewCreateInfo const &imageViewCreateInfo) :
		__device			{ device },
		__commandExecutor	{ commandExecutor },
		__memoryAllocator	{ memoryAllocator },
		__deferredDeleter	{ deferredDeleter }
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
		ImageRegionInfo const &dst,
		void const *pData,
		size_t const size,
		VkPipelineStageFlags2 const beforeStageMask,
		VkAccessFlags2 const beforeAccessMask,
		VkPipelineStageFlags2 const afterStageMask,
		VkAccessFlags2 const afterAccessMask)
	{
		auto pStagingBuffer{ __createStagingBuffer(size) };
		std::memcpy(pStagingBuffer->getData(), pData, size);

		__commandExecutor.reserve([=, &dst{ *__pImage }, &src{ *pStagingBuffer }, regionInfo{ dst }](auto &cmdBuffer)
		{
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
				.subresourceRange		{
					.aspectMask			{ VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT },
					.baseMipLevel		{ 0U },
					.levelCount			{ 1U },
					.baseArrayLayer		{ 0U },
					.layerCount			{ 1U }
				}
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
				.srcBuffer		{ src.getHandle() },
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
				.newLayout				{ VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL },
				.image					{ dst.getHandle() },
				.subresourceRange		{
					.aspectMask			{ VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT },
					.baseMipLevel		{ 0U },
					.levelCount			{ 1U },
					.baseArrayLayer		{ 0U },
					.layerCount			{ 1U }
				}
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

	std::shared_ptr<Dev::MemoryBuffer> Texture::__createStagingBuffer(
		size_t const size)
	{
		VkBufferCreateInfo const bufferCreateInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO },
			.size			{ size },
			.usage			{ VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT },
			.sharingMode	{ VkSharingMode::VK_SHARING_MODE_EXCLUSIVE }
		};

		return std::make_shared<Dev::MemoryBuffer>(
			__device, __memoryAllocator, bufferCreateInfo,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	}
}