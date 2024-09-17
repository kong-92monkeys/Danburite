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

		VkResult vkDeviceWaitIdle();

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

		VkResult vkCreateFramebuffer(
			VkFramebufferCreateInfo const *pCreateInfo,
			VkAllocationCallbacks const *pAllocator,
			VkFramebuffer *pFramebuffer);

		void vkDestroyFramebuffer(
			VkFramebuffer framebuffer,
			VkAllocationCallbacks const *pAllocator);

		VkResult vkCreateFence(
			VkFenceCreateInfo const *pCreateInfo,
			VkAllocationCallbacks const *pAllocator,
			VkFence *pFence);

		void vkDestroyFence(
			VkFence fence,
			VkAllocationCallbacks const *pAllocator);

		VkResult vkWaitForFences(
			uint32_t fenceCount,
			VkFence const *pFences,
			VkBool32 waitAll,
			uint64_t timeout);

		VkResult vkResetFences(
			uint32_t fenceCount,
			VkFence const *pFences);

		VkResult vkCreateSemaphore(
			VkSemaphoreCreateInfo const *pCreateInfo,
			VkAllocationCallbacks const *pAllocator,
			VkSemaphore *pSemaphore);

		void vkDestroySemaphore(
			VkSemaphore semaphore,
			VkAllocationCallbacks const *pAllocator);

		VkResult vkCreateSampler(
			VkSamplerCreateInfo const *pCreateInfo,
			VkAllocationCallbacks const *pAllocator,
			VkSampler *pSampler);

		void vkDestroySampler(
			VkSampler sampler,
			VkAllocationCallbacks const *pAllocator);

		VkResult vkCreateShaderModule(
			VkShaderModuleCreateInfo const *pCreateInfo,
			VkAllocationCallbacks const *pAllocator,
			VkShaderModule *pShaderModule);

		void vkDestroyShaderModule(
			VkShaderModule shaderModule,
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

		VkResult vkCreateDescriptorSetLayout(
			VkDescriptorSetLayoutCreateInfo const *pCreateInfo,
			VkAllocationCallbacks const *pAllocator,
			VkDescriptorSetLayout *pSetLayout);

		void vkDestroyDescriptorSetLayout(
			VkDescriptorSetLayout descriptorSetLayout,
			VkAllocationCallbacks const *pAllocator);

		VkResult vkAllocateDescriptorSets(
			VkDescriptorSetAllocateInfo const *pAllocateInfo,
			VkDescriptorSet *pDescriptorSets);

		VkResult vkFreeDescriptorSets(
			VkDescriptorPool descriptorPool,
			uint32_t descriptorSetCount,
			VkDescriptorSet const *pDescriptorSets);

		void vkUpdateDescriptorSets(
			uint32_t descriptorWriteCount,
			VkWriteDescriptorSet const *pDescriptorWrites,
			uint32_t descriptorCopyCount,
			VkCopyDescriptorSet const *pDescriptorCopies);

		VkResult vkCreatePipelineLayout(
			VkPipelineLayoutCreateInfo const *pCreateInfo,
			VkAllocationCallbacks const *pAllocator,
			VkPipelineLayout *pPipelineLayout);

		void vkDestroyPipelineLayout(
			VkPipelineLayout pipelineLayout,
			VkAllocationCallbacks const *pAllocator);

		VkResult vkCreateGraphicsPipelines(
			VkPipelineCache pipelineCache,
			uint32_t createInfoCount,
			VkGraphicsPipelineCreateInfo const *pCreateInfos,
			VkAllocationCallbacks const *pAllocator,
			VkPipeline *pPipelines);

		VkResult vkCreateComputePipelines(
			VkPipelineCache pipelineCache,
			uint32_t createInfoCount,
			VkComputePipelineCreateInfo const *pCreateInfos,
			VkAllocationCallbacks const *pAllocator,
			VkPipeline *pPipelines);

		void vkDestroyPipeline(
			VkPipeline pipeline,
			VkAllocationCallbacks const *pAllocator);

		VkResult vkCreateRenderPass2(
			VkRenderPassCreateInfo2 const *pCreateInfo,
			VkAllocationCallbacks const *pAllocator,
			VkRenderPass *pRenderPass);

		void vkDestroyRenderPass(
			VkRenderPass renderPass,
			VkAllocationCallbacks const *pAllocator);

		VkResult vkCreateSwapchainKHR(
			VkSwapchainCreateInfoKHR const *pCreateInfo,
			VkAllocationCallbacks const *pAllocator,
			VkSwapchainKHR *pSwapchain);

		void vkDestroySwapchainKHR(
			VkSwapchainKHR swapchain,
			VkAllocationCallbacks const *pAllocator);

		VkResult vkGetSwapchainImagesKHR(
			VkSwapchainKHR swapchain,
			uint32_t *pSwapchainImageCount,
			VkImage *pSwapchainImages);

		VkResult vkAcquireNextImage2KHR(
			VkAcquireNextImageInfoKHR const *pAcquireInfo,
			uint32_t *pImageIndex);

		void vkGetDeviceQueue2(
			VkDeviceQueueInfo2 const *pQueueInfo,
			VkQueue *pQueue);

		VkResult vkQueueWaitIdle(
			VkQueue queue);

		VkResult vkQueueSubmit2(
			VkQueue queue,
			uint32_t submitCount,
			VkSubmitInfo2 const *pSubmits,
			VkFence fence);

		VkResult vkQueuePresentKHR(
			VkQueue queue,
			VkPresentInfoKHR const *pPresentInfo);

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