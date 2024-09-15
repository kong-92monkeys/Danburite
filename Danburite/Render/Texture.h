#pragma once

#include "../Infra/DeferredDeleter.h"
#include "../Vulkan/ImageView.h"
#include "../Device/CommandExecutor.h"
#include "../Device/MemoryBuffer.h"
#include "../Device/MemoryImage.h"

namespace Render
{
	class Texture : public Infra::Unique
	{
	public:
		struct ImageCreateInfo
		{
		public:
			VkImageType imageType{};
			VkFormat format{};
			VkExtent3D extent{};
			uint32_t mipLevels{};
			uint32_t arrayLayers{};
			VkSampleCountFlagBits samples{};
			VkImageTiling tiling{};
			VkImageUsageFlags usage{};
		};

		struct ImageViewCreateInfo
		{
		public:
			VkImageViewType viewType{};
			VkFormat format{};
			VkComponentMapping components{};
			VkImageSubresourceRange subresourceRange{};
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
			Dev::CommandExecutor &instantCommandExecutor,
			Dev::MemoryAllocator &memoryAllocator,
			Infra::DeferredDeleter &deferredDeleter,
			ImageCreateInfo const &imageCreateInfo,
			ImageViewCreateInfo const &imageViewCreateInfo);

		virtual ~Texture() noexcept override;

		[[nodiscard]]
		VK::ImageView const &getImageView() const noexcept;

		void updateData(
			ImageRegionInfo const &dst,
			void const *pData,
			size_t size,
			VkPipelineStageFlags2 beforeStageMask,
			VkAccessFlags2 beforeAccessMask,
			VkPipelineStageFlags2 afterStageMask,
			VkAccessFlags2 afterAccessMask);

	private:
		VK::Device &__device;
		Dev::CommandExecutor &__instantCommandExecutor;
		Dev::MemoryAllocator &__memoryAllocator;
		Infra::DeferredDeleter &__deferredDeleter;

		std::shared_ptr<Dev::MemoryImage> __pImage;
		std::shared_ptr<VK::ImageView> __pImageView;

		void __createImage(
			ImageCreateInfo const &createInfo);

		void __createImageView(
			ImageViewCreateInfo const &createInfo);

		[[nodiscard]]
		std::shared_ptr<Dev::MemoryBuffer> __createStagingBuffer(
			size_t size);
	};
}