#include "TextureUtil.h"
#include "../Infra/Bitmap.h"
#include "../System/Env.h"

namespace Frx::TextureUtil
{
	static constexpr uint32_t __calcMipLevelCountOf(
		Infra::Bitmap const &bitmap) noexcept
	{
		return 1ULL;

		size_t upperSize{ std::max(bitmap.getWidth(), bitmap.getHeight()) };

		uint32_t mipLevels{ };
		while (upperSize)
		{
			++mipLevels;
			upperSize >>= 1ULL;
		}

		return mipLevels;
	}

	Render::Texture *loadTexture(
		Render::Engine &engine,
		std::string_view const &assetPath,
		VkPipelineStageFlags2 const beforeStageMask,
		VkAccessFlags2 const beforeAccessMask,
		VkPipelineStageFlags2 const afterStageMask,
		VkAccessFlags2 const afterAccessMask)
	{
		auto &assetManager		{ Sys::Env::getInstance().getAssetManager() };
		auto const binary		{ assetManager.readBinary(assetPath) };
		Infra::Bitmap bitmap	{ binary.data(), binary.size(), 4ULL };

		auto const mipLevels{ __calcMipLevelCountOf(bitmap) };

		Render::Texture::ImageCreateInfo const imageCreateInfo
		{
			.imageType		{ VkImageType::VK_IMAGE_TYPE_2D },
			.format			{ VkFormat::VK_FORMAT_R8G8B8A8_SRGB },
			.extent			{
				.width		{ static_cast<uint32_t>(bitmap.getWidth()) },
				.height		{ static_cast<uint32_t>(bitmap.getHeight()) },
				.depth		{ 1U }
			},
			.mipLevels		{ mipLevels },
			.arrayLayers	{ 1U },
			.samples		{ VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT },
			.tiling			{ VkImageTiling::VK_IMAGE_TILING_OPTIMAL },
			.usage			{
				VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT |
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

		Render::Texture::ImageRegionInfo const region
		{
			.bufferRowLength		{ static_cast<uint32_t>(bitmap.getWidth()) },
			.bufferImageHeight		{ static_cast<uint32_t>(bitmap.getHeight()) },
			.imageSubresource		{
				.aspectMask			{ VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT },
				.mipLevel			{ 0U },
				.baseArrayLayer		{ 0U },
				.layerCount			{ 1U }
			},
			.imageOffset			{
				.x					{ 0 },
				.y					{ 0 },
				.z					{ 0 }
			},
			.imageExtent			{
				.width				{ static_cast<uint32_t>(bitmap.getWidth()) },
				.height				{ static_cast<uint32_t>(bitmap.getHeight()) },
				.depth				{ 1U }
			}
		};

		auto pRetVal{ engine.createTexture(imageCreateInfo, imageViewCreateInfo) };
		pRetVal->updateData(
			region, bitmap.getData(), bitmap.getDataSize(),
			beforeStageMask, beforeAccessMask, afterStageMask, afterAccessMask,
			VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		return pRetVal;
	}
}