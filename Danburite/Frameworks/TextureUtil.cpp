#include "TextureUtil.h"
#include "../System/Env.h"

namespace Frx::TextureUtil
{
	static constexpr uint32_t __calcMipLevels(
		uint32_t const width,
		uint32_t const height) noexcept
	{
		uint32_t upperSize{ std::max(width, height) };

		uint32_t mipLevels{ };
		while (upperSize)
		{
			++mipLevels;
			upperSize >>= 1U;
		}

		return mipLevels;
	}

	static Render::Texture *__createTexture(
		Render::Engine &engine,
		uint32_t const width,
		uint32_t const height,
		uint32_t const mipLevels)
	{
		Render::Texture::ImageCreateInfo const imageCreateInfo
		{
			.imageType		{ VkImageType::VK_IMAGE_TYPE_2D },
			.format			{ VkFormat::VK_FORMAT_R8G8B8A8_SRGB },
			.extent			{
				.width		{ width },
				.height		{ height },
				.depth		{ 1U }
			},
			.mipLevels		{ mipLevels },
			.arrayLayers	{ 1U },
			.samples		{ VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT },
			.tiling			{ VkImageTiling::VK_IMAGE_TILING_OPTIMAL },
			.usage			{
				VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT |
				VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
				VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT
			}
		};

		Render::Texture::ImageViewCreateInfo const imageViewCreateInfo
		{
			.viewType				{ VkImageViewType::VK_IMAGE_VIEW_TYPE_2D },
			.format					{ VkFormat::VK_FORMAT_R8G8B8A8_SRGB },
			.components				{
				.r					{ VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY },
				.g					{ VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY },
				.b					{ VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY },
				.a					{ VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY }
			},
			.subresourceRange		{
				.aspectMask			{ VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT },
				.baseMipLevel		{ 0U },
				.levelCount			{ mipLevels },
				.baseArrayLayer		{ 0U },
				.layerCount			{ 1U }
			}
		};

		return engine.createTexture(imageCreateInfo, imageViewCreateInfo);
	}

	[[nodiscard]]
	Render::Texture *loadTexture(
		Render::Engine &engine,
		Infra::Bitmap const &bitmap,
		bool const useMipmap,
		VkPipelineStageFlags2 dstStageMask,
		VkAccessFlags2 dstAccessMask)
	{
		uint32_t const bitmapWidth		{ static_cast<uint32_t>(bitmap.getWidth()) };
		uint32_t const bitmapHeight		{ static_cast<uint32_t>(bitmap.getHeight()) };
		uint32_t const mipLevels		{ useMipmap ? __calcMipLevels(bitmapWidth, bitmapHeight) : 1U };

		VkImageSubresourceRange const wholeRange
		{
			.aspectMask			{ VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT },
            .baseMipLevel		{ 0U },
            .levelCount			{ mipLevels },
            .baseArrayLayer		{ 0U },
            .layerCount			{ 1U }
		};

		VkBufferImageCopy2 const rootCopyRegion
		{
			.sType					{ VkStructureType::VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2 },
            .bufferOffset			{ 0U },
			.bufferRowLength		{ bitmapWidth },
			.bufferImageHeight		{ bitmapHeight },
			.imageSubresource		{
				.aspectMask			{ VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT },
				.mipLevel			{ 0U },
				.baseArrayLayer		{ 0U },
				.layerCount			{ 1U }
			},
			.imageOffset			{ 0, 0, 0 },
			.imageExtent			{ bitmapWidth, bitmapHeight, 1U }
		};

		auto const pRetVal{ __createTexture(engine, bitmapWidth, bitmapHeight, mipLevels) };

		pRetVal->transitLayout(
			VK_PIPELINE_STAGE_2_NONE, VK_ACCESS_2_NONE,
			VK_PIPELINE_STAGE_2_COPY_BIT, VK_ACCESS_2_TRANSFER_WRITE_BIT,
			VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
			VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			wholeRange);

		pRetVal->updateData(
			rootCopyRegion,
			VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			bitmap.getData(), bitmap.getDataSize());

		VkOffset3D srcSize
		{
			static_cast<int>(bitmapWidth),
			static_cast<int>(bitmapHeight),
			1
		};

		for (uint32_t mipIter{ }; mipIter < (mipLevels - 1U); ++mipIter)
		{
			VkImageSubresourceRange const srcRange
			{
				.aspectMask			{ VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT },
				.baseMipLevel		{ mipIter },
				.levelCount			{ 1U },
				.baseArrayLayer		{ 0U },
				.layerCount			{ 1U }
			};

			VkOffset3D const dstSize
			{
				srcSize.x >> 1,
				srcSize.y >> 1,
				srcSize.z
			};

			VkImageBlit blitInfo{ };

			blitInfo.srcSubresource.aspectMask			= VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
			blitInfo.srcSubresource.mipLevel			= mipIter;
			blitInfo.srcSubresource.baseArrayLayer		= 0U;
			blitInfo.srcSubresource.layerCount			= 1U;
			blitInfo.srcOffsets[1]						= srcSize;

			blitInfo.dstSubresource.aspectMask			= VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
			blitInfo.dstSubresource.mipLevel			= (mipIter + 1U);
			blitInfo.dstSubresource.baseArrayLayer		= 0U;
			blitInfo.dstSubresource.layerCount			= 1U;
			blitInfo.dstOffsets[1]						= dstSize;

			pRetVal->transitLayout(
				VK_PIPELINE_STAGE_2_COPY_BIT, VK_ACCESS_2_TRANSFER_WRITE_BIT,
				VK_PIPELINE_STAGE_2_COPY_BIT, VK_ACCESS_2_TRANSFER_READ_BIT,
				VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				srcRange);

			pRetVal->blit(
				VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				blitInfo, VkFilter::VK_FILTER_LINEAR);

			pRetVal->transitLayout(
				VK_PIPELINE_STAGE_2_COPY_BIT, VK_ACCESS_2_TRANSFER_READ_BIT,
				dstStageMask, dstAccessMask,
				VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				srcRange);

			srcSize = dstSize;
		}

		VkImageSubresourceRange const lastRange
		{
			.aspectMask			{ VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT },
            .baseMipLevel		{ (mipLevels - 1U) },
            .levelCount			{ 1U },
            .baseArrayLayer		{ 0U },
            .layerCount			{ 1U }
		};

		pRetVal->transitLayout(
			VK_PIPELINE_STAGE_2_COPY_BIT, VK_ACCESS_2_TRANSFER_READ_BIT,
			dstStageMask, dstAccessMask,
			VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			lastRange);

		return pRetVal;
	}

	Render::Texture *loadTexture(
		Render::Engine &engine,
		std::string_view const &assetPath,
		bool const useMipmap,
		VkPipelineStageFlags2 const dstStageMask,
		VkAccessFlags2 const dstAccessMask)
	{
		auto &assetManager		{ Sys::Env::getInstance().getAssetManager() };
		auto const binary		{ assetManager.readBinary(assetPath) };
		Infra::Bitmap bitmap	{ binary.data(), binary.size(), 4ULL };

		return loadTexture(
			engine, bitmap, useMipmap,
			dstStageMask, dstAccessMask);
	}
}