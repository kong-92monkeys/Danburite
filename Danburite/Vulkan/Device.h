#pragma once

#include "PhysicalDevice.h"

namespace VK
{
	class Device : public Infra::Handle<VkDevice>
	{
	public:
		Device(
			PhysicalDevice &physicalDevice,
			VkDeviceCreateInfo const &createInfo);

		virtual ~Device() noexcept override;

		VkResult vkCreatePipelineCache(
			VkPipelineCacheCreateInfo const *pCreateInfo,
			VkAllocationCallbacks const *pAllocator,
			VkPipelineCache *pPipelineCache);

		void vkDestroyPipelineCache(
			VkPipelineCache pipelineCache,
			VkAllocationCallbacks const *pAllocator);

		VkResult vkAllocateMemory(
			VkMemoryAllocateInfo const *pAllocateInfo,
			VkAllocationCallbacks const *pAllocator,
			VkDeviceMemory *pMemory);

		void vkFreeMemory(
			VkDeviceMemory memory,
			VkAllocationCallbacks const *pAllocator);

		VkResult vkMapMemory(
			VkDeviceMemory memory,
			VkDeviceSize offset,
			VkDeviceSize size,
			VkMemoryMapFlags flags,
			void **ppData);

		void vkUnmapMemory(
			VkDeviceMemory memory);

		VkResult vkCreateBuffer(
			VkBufferCreateInfo const *pCreateInfo,
			VkAllocationCallbacks const *pAllocator,
			VkBuffer *pBuffer);

		void vkDestroyBuffer(
			VkBuffer buffer,
			VkAllocationCallbacks const *pAllocator);

		void vkGetBufferMemoryRequirements2(
			VkBufferMemoryRequirementsInfo2 const *pInfo,
			VkMemoryRequirements2 *pMemoryRequirements) const;

		VkResult vkBindBufferMemory2(
			uint32_t bindInfoCount,
			VkBindBufferMemoryInfo const *pBindInfos);

		VkResult vkCreateImage(
			VkImageCreateInfo const *pCreateInfo,
			VkAllocationCallbacks const *pAllocator,
			VkImage *pImage);

		void vkDestroyImage(
			VkImage image,
			VkAllocationCallbacks const *pAllocator);

		void vkGetImageMemoryRequirements2(
			VkImageMemoryRequirementsInfo2 const *pInfo,
			VkMemoryRequirements2 *pMemoryRequirements) const;

		VkResult vkBindImageMemory2(
			uint32_t bindInfoCount,
			VkBindImageMemoryInfo const *pBindInfos);

		VkResult vkCreateImageView(
			VkImageViewCreateInfo const *pCreateInfo,
			VkAllocationCallbacks const *pAllocator,
			VkImageView *pView);

		void vkDestroyImageView(
			VkImageView imageView,
			VkAllocationCallbacks const *pAllocator);

	private:
		PhysicalDevice &__physicalDevice;

		VK::DeviceProc __deviceProc;

		void __loadDeviceProc() noexcept;

		[[nodiscard]]
		static VkDevice __create(
			PhysicalDevice &physicalDevice,
			VkDeviceCreateInfo const &createInfo);
	};
}