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

		VkResult vkCreateDescriptorPool(
			VkDescriptorPoolCreateInfo const *pCreateInfo,
			VkAllocationCallbacks const *pAllocator,
			VkDescriptorPool *pDescriptorPool);

		void vkDestroyDescriptorPool(
			VkDescriptorPool descriptorPool,
			VkAllocationCallbacks const *pAllocator);

		VkResult vkResetDescriptorPool(
			VkDescriptorPool descriptorPool,
			VkDescriptorPoolResetFlags flags);

		VkResult vkCreateCommandPool(
			VkCommandPoolCreateInfo const *pCreateInfo,
			VkAllocationCallbacks const *pAllocator,
			VkCommandPool *pCommandPool);

		void vkDestroyCommandPool(
			VkCommandPool commandPool,
			VkAllocationCallbacks const *pAllocator);

		VkResult vkAllocateCommandBuffers(
			VkCommandBufferAllocateInfo const *pAllocateInfo,
			VkCommandBuffer *pCommandBuffers);

		VkResult vkResetCommandPool(
			VkCommandPool commandPool,
			VkCommandPoolResetFlags flags);

		VkResult vkBeginCommandBuffer(
			VkCommandBuffer commandBuffer,
			VkCommandBufferBeginInfo const *pBeginInfo);

		VkResult vkEndCommandBuffer(
			VkCommandBuffer commandBuffer);

		void vkCmdBeginRenderPass2(
			VkCommandBuffer commandBuffer,
			VkRenderPassBeginInfo const *pRenderPassBegin,
			VkSubpassBeginInfo const *pSubpassBeginInfo);

		void vkCmdEndRenderPass2(
			VkCommandBuffer commandBuffer,
			VkSubpassEndInfo const *pSubpassEndInfo);

		void vkCmdBindPipeline(
			VkCommandBuffer commandBuffer,
			VkPipelineBindPoint pipelineBindPoint,
			VkPipeline pipeline);

		void vkCmdSetViewport(
			VkCommandBuffer commandBuffer,
			uint32_t firstViewport,
			uint32_t viewportCount,
			VkViewport const *pViewports);

		void vkCmdSetScissor(
			VkCommandBuffer commandBuffer,
			uint32_t firstScissor,
			uint32_t scissorCount,
			VkRect2D const *pScissors);

		void vkCmdDraw(
			VkCommandBuffer commandBuffer,
			uint32_t vertexCount,
			uint32_t instanceCount,
			uint32_t firstVertex,
			uint32_t firstInstance);

		void vkCmdDrawIndexed(
			VkCommandBuffer commandBuffer,
			uint32_t indexCount,
			uint32_t instanceCount,
			uint32_t firstIndex,
			int32_t vertexOffset,
			uint32_t firstInstance);

		void vkCmdCopyBuffer2(
			VkCommandBuffer commandBuffer,
			VkCopyBufferInfo2 const *pCopyBufferInfo);

		void vkCmdCopyBufferToImage2(
			VkCommandBuffer commandBuffer,
			VkCopyBufferToImageInfo2 const *pCopyBufferToImageInfo);

		void vkCmdPipelineBarrier2(
			VkCommandBuffer commandBuffer,
			VkDependencyInfo const *pDependencyInfo);

		void vkCmdBindVertexBuffers(
			VkCommandBuffer commandBuffer,
			uint32_t firstBinding,
			uint32_t bindingCount,
			VkBuffer const *pBuffers,
			VkDeviceSize const *pOffsets);

		void vkCmdBindIndexBuffer(
			VkCommandBuffer commandBuffer,
			VkBuffer buffer,
			VkDeviceSize offset,
			VkIndexType indexType);

		void vkCmdBindDescriptorSets(
			VkCommandBuffer commandBuffer,
			VkPipelineBindPoint pipelineBindPoint,
			VkPipelineLayout layout,
			uint32_t firstSet,
			uint32_t descriptorSetCount,
			VkDescriptorSet const *pDescriptorSets,
			uint32_t dynamicOffsetCount,
			uint32_t const *pDynamicOffsets);

		void vkCmdExecuteCommands(
			VkCommandBuffer commandBuffer,
			uint32_t commandBufferCount,
			VkCommandBuffer const *pCommandBuffers);

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