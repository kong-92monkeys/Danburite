#pragma once

#include "../Infra/DeferredDeleter.h"
#include "../Vulkan/ImageView.h"
#include "../Device/CommandExecutor.h"
#include "../Device/MemoryBuffer.h"
#include "../Device/MemoryImage.h"
#include "ResourcePool.h"

namespace Render
{
	class Texture : public Infra::Unique
	{
	public:
		struct ImageCreateInfo
		{
		public:
			VkImageType imageType{ };
			VkFormat format{ };
			VkExtent3D extent{ };
			uint32_t mipLevels{ };
			uint32_t arrayLayers{ };
			VkSampleCountFlagBits samples{ };
			VkImageTiling tiling{ };
			VkImageUsageFlags usage{ };
		};

		struct ImageViewCreateInfo
		{
		public:
			VkImageViewType viewType{ };
			VkFormat format{ };
			VkComponentMapping components{ };
			VkImageSubresourceRange subresourceRange{ };
		};

		struct ImageRegionInfo
		{
		public:
			uint32_t bufferRowLength{ };
			uint32_t bufferImageHeight{ };
			VkImageSubresourceLayers imageSubresource{ };
			VkOffset3D imageOffset{ };
			VkExtent3D imageExtent{ };
		};

		Texture(
			VK::Device &device,
			Dev::CommandExecutor &commandExecutor,
			Dev::MemoryAllocator &memoryAllocator,
			Infra::DeferredDeleter &deferredDeleter,
			ResourcePool &resourcePool,
			ImageCreateInfo const &imageCreateInfo,
			ImageViewCreateInfo const &imageViewCreateInfo);

		virtual ~Texture() noexcept override;

		[[nodiscard]]
		VK::ImageView const &getImageView() const noexcept;

		void transitLayout(
			VkPipelineStageFlags2 srcStageMask,
			VkAccessFlags2 srcAccessMask,
			VkPipelineStageFlags2 dstStageMask,
			VkAccessFlags2 dstAccessMask,
			VkImageLayout oldLayout,
			VkImageLayout newLayout,
			VkImageSubresourceRange const &subresourceRange);

		void updateData(
			VkBufferImageCopy2 const &region,
			VkImageLayout imageLayout,
			void const *pData,
			size_t size);

		void blit(
			VkImageLayout srcImageLayout,
			VkImageLayout dstImageLayout,
			VkImageBlit const &region,
			VkFilter filter);

	private:
		VK::Device &__device;
		Dev::CommandExecutor &__commandExecutor;
		Dev::MemoryAllocator &__memoryAllocator;
		Infra::DeferredDeleter &__deferredDeleter;
		ResourcePool &__resourcePool;

		std::shared_ptr<Dev::MemoryImage> __pImage;
		std::shared_ptr<VK::ImageView> __pImageView;

		void __createImage(
			ImageCreateInfo const &createInfo);

		void __createImageView(
			ImageViewCreateInfo const &createInfo);
	};
}