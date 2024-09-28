#pragma warning(disable: 5103)

#include "Device.h"
#include <stdexcept>

#define LOAD_DEVICE_PROC(funcName) (__deviceProc.##funcName = reinterpret_cast<PFN_##funcName>(__physicalDevice.vkGetDeviceProcAddr(getHandle(), #funcName)))

namespace VK
{
	Device::Device(
		PhysicalDevice const &physicalDevice,
		VkDeviceCreateInfo const &createInfo) :
		Handle				{ __create(physicalDevice, createInfo) },
		__physicalDevice	{ physicalDevice }
	{
		__loadDeviceProc();
	}

	Device::~Device() noexcept
	{
		__deviceProc.vkDestroyDevice(getHandle(), nullptr);
	}

	VkResult Device::vkDeviceWaitIdle()
	{
		return __deviceProc.vkDeviceWaitIdle(getHandle());
	}

	VkResult Device::vkCreatePipelineCache(
		VkPipelineCacheCreateInfo const *const pCreateInfo,
		VkAllocationCallbacks const *const pAllocator,
		VkPipelineCache *const pPipelineCache)
	{
		return __deviceProc.vkCreatePipelineCache(
			getHandle(), pCreateInfo, pAllocator, pPipelineCache);
	}

	void Device::vkDestroyPipelineCache(
		VkPipelineCache const pipelineCache,
		VkAllocationCallbacks const *const pAllocator)
	{
		__deviceProc.vkDestroyPipelineCache(
			getHandle(), pipelineCache, pAllocator);
	}

	VkResult Device::vkAllocateMemory(
		VkMemoryAllocateInfo const *const pAllocateInfo,
		VkAllocationCallbacks const *const pAllocator,
		VkDeviceMemory *const pMemory)
	{
		return __deviceProc.vkAllocateMemory(
			getHandle(), pAllocateInfo, pAllocator, pMemory);
	}

	void Device::vkFreeMemory(
		VkDeviceMemory const memory,
		VkAllocationCallbacks const *const pAllocator)
	{
		__deviceProc.vkFreeMemory(
			getHandle(), memory, pAllocator);
	}

	VkResult Device::vkMapMemory(
		VkDeviceMemory const memory,
		VkDeviceSize const offset,
		VkDeviceSize const size,
		VkMemoryMapFlags const flags,
		void **const ppData)
	{
		return __deviceProc.vkMapMemory(
			getHandle(), memory, offset, size, flags, ppData);
	}

	void Device::vkUnmapMemory(
		VkDeviceMemory const memory)
	{
		__deviceProc.vkUnmapMemory(
			getHandle(), memory);
	}

	VkResult Device::vkCreateBuffer(
		VkBufferCreateInfo const *const pCreateInfo,
		VkAllocationCallbacks const *const pAllocator,
		VkBuffer *const pBuffer)
	{
		return __deviceProc.vkCreateBuffer(
			getHandle(), pCreateInfo, pAllocator, pBuffer);
	}

	void Device::vkDestroyBuffer(
		VkBuffer const buffer,
		VkAllocationCallbacks const *const pAllocator)
	{
		__deviceProc.vkDestroyBuffer(
			getHandle(), buffer, pAllocator);
	}

	void Device::vkGetBufferMemoryRequirements2(
		VkBufferMemoryRequirementsInfo2 const *const pInfo,
		VkMemoryRequirements2 *const pMemoryRequirements) const
	{
		__deviceProc.vkGetBufferMemoryRequirements2(
			getHandle(), pInfo, pMemoryRequirements);
	}

	VkResult Device::vkBindBufferMemory2(
		uint32_t const bindInfoCount,
		VkBindBufferMemoryInfo const *const pBindInfos)
	{
		return __deviceProc.vkBindBufferMemory2(
			getHandle(), bindInfoCount, pBindInfos);
	}

	VkResult Device::vkCreateImage(
		VkImageCreateInfo const *const pCreateInfo,
		VkAllocationCallbacks const *const pAllocator,
		VkImage *const pImage)
	{
		return __deviceProc.vkCreateImage(
			getHandle(), pCreateInfo, pAllocator, pImage);
	}

	void Device::vkDestroyImage(
		VkImage const image,
		VkAllocationCallbacks const *const pAllocator)
	{
		__deviceProc.vkDestroyImage(
			getHandle(), image, pAllocator);
	}

	void Device::vkGetImageMemoryRequirements2(
		VkImageMemoryRequirementsInfo2 const *const pInfo,
		VkMemoryRequirements2 *const pMemoryRequirements) const
	{
		__deviceProc.vkGetImageMemoryRequirements2(
			getHandle(), pInfo, pMemoryRequirements);
	}

	VkResult Device::vkBindImageMemory2(
		uint32_t const bindInfoCount,
		VkBindImageMemoryInfo const *const pBindInfos)
	{
		return __deviceProc.vkBindImageMemory2(
			getHandle(), bindInfoCount, pBindInfos);
	}

	VkResult Device::vkCreateImageView(
		VkImageViewCreateInfo const *const pCreateInfo,
		VkAllocationCallbacks const *const pAllocator,
		VkImageView *const pView)
	{
		return __deviceProc.vkCreateImageView(
			getHandle(), pCreateInfo, pAllocator, pView);
	}

	void Device::vkDestroyImageView(
		VkImageView const imageView,
		VkAllocationCallbacks const *const pAllocator)
	{
		__deviceProc.vkDestroyImageView(
			getHandle(), imageView, pAllocator);
	}

	VkResult Device::vkCreateFramebuffer(
		VkFramebufferCreateInfo const *const pCreateInfo,
		VkAllocationCallbacks const *const pAllocator,
		VkFramebuffer *const pFramebuffer)
	{
		return __deviceProc.vkCreateFramebuffer(
			getHandle(), pCreateInfo, pAllocator, pFramebuffer);
	}

	void Device::vkDestroyFramebuffer(
		VkFramebuffer const framebuffer,
		VkAllocationCallbacks const *const pAllocator)
	{
		__deviceProc.vkDestroyFramebuffer(
			getHandle(), framebuffer, pAllocator);
	}

	VkResult Device::vkCreateFence(
		VkFenceCreateInfo const *const pCreateInfo,
		VkAllocationCallbacks const *const pAllocator,
		VkFence *const pFence)
	{
		return __deviceProc.vkCreateFence(
			getHandle(), pCreateInfo, pAllocator, pFence);
	}

	void Device::vkDestroyFence(
		VkFence const fence,
		VkAllocationCallbacks const *const pAllocator)
	{
		__deviceProc.vkDestroyFence(
			getHandle(), fence, pAllocator);
	}

	VkResult Device::vkWaitForFences(
		uint32_t const fenceCount,
		VkFence const *const pFences,
		VkBool32 const waitAll,
		uint64_t const timeout)
	{
		return __deviceProc.vkWaitForFences(
			getHandle(), fenceCount, pFences, waitAll, timeout);
	}

	VkResult Device::vkResetFences(
		uint32_t const fenceCount,
		VkFence const *const pFences)
	{
		return __deviceProc.vkResetFences(
			getHandle(), fenceCount, pFences);
	}

	VkResult Device::vkCreateSemaphore(
		VkSemaphoreCreateInfo const *const pCreateInfo,
		VkAllocationCallbacks const *const pAllocator,
		VkSemaphore *const pSemaphore)
	{
		return __deviceProc.vkCreateSemaphore(
			getHandle(), pCreateInfo, pAllocator, pSemaphore);
	}

	void Device::vkDestroySemaphore(
		VkSemaphore const semaphore,
		VkAllocationCallbacks const *const pAllocator)
	{
		__deviceProc.vkDestroySemaphore(
			getHandle(), semaphore, pAllocator);
	}

	VkResult Device::vkCreateSampler(
		VkSamplerCreateInfo const *const pCreateInfo,
		VkAllocationCallbacks const *const pAllocator,
		VkSampler *const pSampler)
	{
		return __deviceProc.vkCreateSampler(
			getHandle(), pCreateInfo, pAllocator, pSampler);
	}

	void Device::vkDestroySampler(
		VkSampler const sampler,
		VkAllocationCallbacks const *const pAllocator)
	{
		__deviceProc.vkDestroySampler(
			getHandle(), sampler, pAllocator);
	}

	VkResult Device::vkCreateShaderModule(
		VkShaderModuleCreateInfo const *const pCreateInfo,
		VkAllocationCallbacks const *const pAllocator,
		VkShaderModule *const pShaderModule)
	{
		return __deviceProc.vkCreateShaderModule(
			getHandle(), pCreateInfo, pAllocator, pShaderModule);
	}

	void Device::vkDestroyShaderModule(
		VkShaderModule const shaderModule,
		VkAllocationCallbacks const *const pAllocator)
	{
		__deviceProc.vkDestroyShaderModule(
			getHandle(), shaderModule, pAllocator);
	}

	VkResult Device::vkCreateDescriptorPool(
		VkDescriptorPoolCreateInfo const *const pCreateInfo,
		VkAllocationCallbacks const *const pAllocator,
		VkDescriptorPool *const pDescriptorPool)
	{
		return __deviceProc.vkCreateDescriptorPool(
			getHandle(), pCreateInfo, pAllocator, pDescriptorPool);
	}

	void Device::vkDestroyDescriptorPool(
		VkDescriptorPool const descriptorPool,
		VkAllocationCallbacks const *const pAllocator)
	{
		__deviceProc.vkDestroyDescriptorPool(
			getHandle(), descriptorPool, pAllocator);
	}

	VkResult Device::vkResetDescriptorPool(
		VkDescriptorPool const descriptorPool,
		VkDescriptorPoolResetFlags const flags)
	{
		return __deviceProc.vkResetDescriptorPool(
			getHandle(), descriptorPool, flags);
	}

	VkResult Device::vkCreateDescriptorSetLayout(
		VkDescriptorSetLayoutCreateInfo const *const pCreateInfo,
		VkAllocationCallbacks const *const pAllocator,
		VkDescriptorSetLayout *const pSetLayout)
	{
		return __deviceProc.vkCreateDescriptorSetLayout(
			getHandle(), pCreateInfo, pAllocator, pSetLayout);
	}

	void Device::vkDestroyDescriptorSetLayout(
		VkDescriptorSetLayout const descriptorSetLayout,
		VkAllocationCallbacks const *const pAllocator)
	{
		__deviceProc.vkDestroyDescriptorSetLayout(
			getHandle(), descriptorSetLayout, pAllocator);
	}

	VkResult Device::vkAllocateDescriptorSets(
		VkDescriptorSetAllocateInfo const *const pAllocateInfo,
		VkDescriptorSet *const pDescriptorSets)
	{
		return __deviceProc.vkAllocateDescriptorSets(
			getHandle(), pAllocateInfo, pDescriptorSets);
	}

	VkResult Device::vkFreeDescriptorSets(
		VkDescriptorPool const descriptorPool,
		uint32_t const descriptorSetCount,
		VkDescriptorSet const *const pDescriptorSets)
	{
		return __deviceProc.vkFreeDescriptorSets(
			getHandle(), descriptorPool, descriptorSetCount, pDescriptorSets);
	}

	void Device::vkUpdateDescriptorSets(
		uint32_t const descriptorWriteCount,
		VkWriteDescriptorSet const *const pDescriptorWrites,
		uint32_t const descriptorCopyCount,
		VkCopyDescriptorSet const *const pDescriptorCopies)
	{
		__deviceProc.vkUpdateDescriptorSets(
			getHandle(),
			descriptorWriteCount, pDescriptorWrites,
			descriptorCopyCount, pDescriptorCopies);
	}

	VkResult Device::vkCreatePipelineLayout(
		VkPipelineLayoutCreateInfo const *const pCreateInfo,
		VkAllocationCallbacks const *const pAllocator,
		VkPipelineLayout *const pPipelineLayout)
	{
		return __deviceProc.vkCreatePipelineLayout(
			getHandle(), pCreateInfo, pAllocator, pPipelineLayout);
	}

	void Device::vkDestroyPipelineLayout(
		VkPipelineLayout const pipelineLayout,
		VkAllocationCallbacks const *const pAllocator)
	{
		__deviceProc.vkDestroyPipelineLayout(
			getHandle(), pipelineLayout, pAllocator);
	}

	VkResult Device::vkCreateGraphicsPipelines(
		VkPipelineCache const pipelineCache,
		uint32_t const createInfoCount,
		VkGraphicsPipelineCreateInfo const *const pCreateInfos,
		VkAllocationCallbacks const *const pAllocator,
		VkPipeline *const pPipelines)
	{
		return __deviceProc.vkCreateGraphicsPipelines(
			getHandle(), pipelineCache,
			createInfoCount, pCreateInfos, pAllocator, pPipelines);
	}

	VkResult Device::vkCreateComputePipelines(
		VkPipelineCache const pipelineCache,
		uint32_t const createInfoCount,
		VkComputePipelineCreateInfo const *const pCreateInfos,
		VkAllocationCallbacks const *const pAllocator,
		VkPipeline *const pPipelines)
	{
		return __deviceProc.vkCreateComputePipelines(
			getHandle(), pipelineCache,
			createInfoCount, pCreateInfos, pAllocator, pPipelines);
	}

	void Device::vkDestroyPipeline(
		VkPipeline const pipeline,
		VkAllocationCallbacks const *const pAllocator)
	{
		__deviceProc.vkDestroyPipeline(
			getHandle(), pipeline, pAllocator);
	}

	VkResult Device::vkCreateRenderPass2(
		VkRenderPassCreateInfo2 const *const pCreateInfo,
		VkAllocationCallbacks const *const pAllocator,
		VkRenderPass *const pRenderPass)
	{
		return __deviceProc.vkCreateRenderPass2(
			getHandle(), pCreateInfo, pAllocator, pRenderPass);
	}

	void Device::vkDestroyRenderPass(
		VkRenderPass const renderPass,
		VkAllocationCallbacks const *const pAllocator)
	{
		__deviceProc.vkDestroyRenderPass(
			getHandle(), renderPass, pAllocator);
	}

	VkResult Device::vkCreateSwapchainKHR(
		VkSwapchainCreateInfoKHR const *const pCreateInfo,
		VkAllocationCallbacks const *const pAllocator,
		VkSwapchainKHR *const pSwapchain)
	{
		return __deviceProc.vkCreateSwapchainKHR(
			getHandle(), pCreateInfo, pAllocator, pSwapchain);
	}

	void Device::vkDestroySwapchainKHR(
		VkSwapchainKHR const swapchain,
		VkAllocationCallbacks const *const pAllocator)
	{
		__deviceProc.vkDestroySwapchainKHR(
			getHandle(), swapchain, pAllocator);
	}

	VkResult Device::vkGetSwapchainImagesKHR(
		VkSwapchainKHR const swapchain,
		uint32_t *const pSwapchainImageCount,
		VkImage *const pSwapchainImages)
	{
		return __deviceProc.vkGetSwapchainImagesKHR(
			getHandle(), swapchain, pSwapchainImageCount, pSwapchainImages);
	}

	VkResult Device::vkAcquireNextImage2KHR(
		VkAcquireNextImageInfoKHR const *const pAcquireInfo,
		uint32_t *const pImageIndex)
	{
		return __deviceProc.vkAcquireNextImage2KHR(
			getHandle(), pAcquireInfo, pImageIndex);
	}

	void Device::vkGetDeviceQueue2(
		VkDeviceQueueInfo2 const *const pQueueInfo,
		VkQueue *const pQueue)
	{
		__deviceProc.vkGetDeviceQueue2(
			getHandle(), pQueueInfo, pQueue);
	}

	VkResult Device::vkQueueWaitIdle(
		VkQueue const queue)
	{
		return __deviceProc.vkQueueWaitIdle(queue);
	}

	VkResult Device::vkQueueSubmit2(
		VkQueue const queue,
		uint32_t const submitCount,
		VkSubmitInfo2 const *const pSubmits,
		VkFence const fence)
	{
		return __deviceProc.vkQueueSubmit2(
			queue, submitCount, pSubmits, fence);
	}

	VkResult Device::vkQueuePresentKHR(
		VkQueue const queue,
		VkPresentInfoKHR const *const pPresentInfo)
	{
		return __deviceProc.vkQueuePresentKHR(
			queue, pPresentInfo);
	}

	VkResult Device::vkCreateCommandPool(
		VkCommandPoolCreateInfo const *const pCreateInfo,
		VkAllocationCallbacks const *const pAllocator,
		VkCommandPool *const pCommandPool)
	{
		return __deviceProc.vkCreateCommandPool(
			getHandle(), pCreateInfo, pAllocator, pCommandPool);
	}

	void Device::vkDestroyCommandPool(
		VkCommandPool const commandPool,
		VkAllocationCallbacks const *const pAllocator)
	{
		__deviceProc.vkDestroyCommandPool(
			getHandle(), commandPool, pAllocator);
	}

	VkResult Device::vkAllocateCommandBuffers(
		VkCommandBufferAllocateInfo const *const pAllocateInfo,
		VkCommandBuffer *const pCommandBuffers)
	{
		return __deviceProc.vkAllocateCommandBuffers(
			getHandle(), pAllocateInfo, pCommandBuffers);
	}

	VkResult Device::vkResetCommandPool(
		VkCommandPool const commandPool,
		VkCommandPoolResetFlags const flags)
	{
		return __deviceProc.vkResetCommandPool(
			getHandle(), commandPool, flags);
	}

	VkResult Device::vkBeginCommandBuffer(
		VkCommandBuffer const commandBuffer,
		VkCommandBufferBeginInfo const *const pBeginInfo)
	{
		return __deviceProc.vkBeginCommandBuffer(
			commandBuffer, pBeginInfo);
	}

	VkResult Device::vkEndCommandBuffer(
		VkCommandBuffer const commandBuffer)
	{
		return __deviceProc.vkEndCommandBuffer(
			commandBuffer);
	}

	void Device::vkCmdBeginRenderPass2(
		VkCommandBuffer const commandBuffer,
		VkRenderPassBeginInfo const *const pRenderPassBegin,
		VkSubpassBeginInfo const *const pSubpassBeginInfo)
	{
		__deviceProc.vkCmdBeginRenderPass2(
			commandBuffer, pRenderPassBegin, pSubpassBeginInfo);
	}

	void Device::vkCmdEndRenderPass2(
		VkCommandBuffer const commandBuffer,
		VkSubpassEndInfo const *const pSubpassEndInfo)
	{
		__deviceProc.vkCmdEndRenderPass2(
			commandBuffer, pSubpassEndInfo);
	}

	void Device::vkCmdBindPipeline(
		VkCommandBuffer const commandBuffer,
		VkPipelineBindPoint const pipelineBindPoint,
		VkPipeline const pipeline)
	{
		__deviceProc.vkCmdBindPipeline(
			commandBuffer, pipelineBindPoint, pipeline);
	}

	void Device::vkCmdSetViewport(
		VkCommandBuffer const commandBuffer,
		uint32_t const firstViewport,
		uint32_t const viewportCount,
		VkViewport const *const pViewports)
	{
		__deviceProc.vkCmdSetViewport(
			commandBuffer, firstViewport, viewportCount, pViewports);
	}

	void Device::vkCmdSetScissor(
		VkCommandBuffer const commandBuffer,
		uint32_t const firstScissor,
		uint32_t const scissorCount,
		VkRect2D const *const pScissors)
	{
		__deviceProc.vkCmdSetScissor(
			commandBuffer, firstScissor, scissorCount, pScissors);
	}

	void Device::vkCmdDraw(
		VkCommandBuffer const commandBuffer,
		uint32_t const vertexCount,
		uint32_t const instanceCount,
		uint32_t const firstVertex,
		uint32_t const firstInstance)
	{
		__deviceProc.vkCmdDraw(
			commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
	}

	void Device::vkCmdDrawIndexed(
		VkCommandBuffer const commandBuffer,
		uint32_t const indexCount,
		uint32_t const instanceCount,
		uint32_t const firstIndex,
		int32_t const vertexOffset,
		uint32_t const firstInstance)
	{
		__deviceProc.vkCmdDrawIndexed(
			commandBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
	}

	void Device::vkCmdCopyBuffer2(
		VkCommandBuffer const commandBuffer,
		VkCopyBufferInfo2 const *const pCopyBufferInfo)
	{
		__deviceProc.vkCmdCopyBuffer2(
			commandBuffer, pCopyBufferInfo);
	}

	void Device::vkCmdCopyBufferToImage2(
		VkCommandBuffer const commandBuffer,
		VkCopyBufferToImageInfo2 const *const pCopyBufferToImageInfo)
	{
		__deviceProc.vkCmdCopyBufferToImage2(
			commandBuffer, pCopyBufferToImageInfo);
	}

	void Device::vkCmdPipelineBarrier2(
		VkCommandBuffer const commandBuffer,
		VkDependencyInfo const *const pDependencyInfo)
	{
		__deviceProc.vkCmdPipelineBarrier2(
			commandBuffer, pDependencyInfo);
	}

	void Device::vkCmdBindVertexBuffers(
		VkCommandBuffer const commandBuffer,
		uint32_t const firstBinding,
		uint32_t const bindingCount,
		VkBuffer const *const pBuffers,
		VkDeviceSize const *const pOffsets)
	{
		__deviceProc.vkCmdBindVertexBuffers(
			commandBuffer, firstBinding, bindingCount, pBuffers, pOffsets);
	}

	void Device::vkCmdBindIndexBuffer(
		VkCommandBuffer const commandBuffer,
		VkBuffer const buffer,
		VkDeviceSize const offset,
		VkIndexType const indexType)
	{
		__deviceProc.vkCmdBindIndexBuffer(
			commandBuffer, buffer, offset, indexType);
	}

	void Device::vkCmdBindDescriptorSets(
		VkCommandBuffer const commandBuffer,
		VkPipelineBindPoint const pipelineBindPoint,
		VkPipelineLayout const layout,
		uint32_t const firstSet,
		uint32_t const descriptorSetCount,
		VkDescriptorSet const *const pDescriptorSets,
		uint32_t const dynamicOffsetCount,
		uint32_t const *const pDynamicOffsets)
	{
		__deviceProc.vkCmdBindDescriptorSets(
			commandBuffer, pipelineBindPoint, layout,
			firstSet, descriptorSetCount, pDescriptorSets,
			dynamicOffsetCount, pDynamicOffsets);
	}

	void Device::vkCmdExecuteCommands(
		VkCommandBuffer const commandBuffer,
		uint32_t const commandBufferCount,
		VkCommandBuffer const *const pCommandBuffers)
	{
		__deviceProc.vkCmdExecuteCommands(
			commandBuffer, commandBufferCount, pCommandBuffers);
	}

	void Device::vkCmdSetVertexInputEXT(
		VkCommandBuffer const commandBuffer,
		uint32_t const vertexBindingDescriptionCount,
		VkVertexInputBindingDescription2EXT const *const pVertexBindingDescriptions,
		uint32_t const vertexAttributeDescriptionCount,
		VkVertexInputAttributeDescription2EXT const *const pVertexAttributeDescriptions)
	{
		__deviceProc.vkCmdSetVertexInputEXT(
			commandBuffer,
			vertexBindingDescriptionCount, pVertexBindingDescriptions,
			vertexAttributeDescriptionCount, pVertexAttributeDescriptions);
	}

	void Device::__loadDeviceProc() noexcept
	{
		// Device
		LOAD_DEVICE_PROC(vkDeviceWaitIdle);
		LOAD_DEVICE_PROC(vkDestroyDevice);

		// Queue
		LOAD_DEVICE_PROC(vkGetDeviceQueue2);
		LOAD_DEVICE_PROC(vkQueueWaitIdle);
		LOAD_DEVICE_PROC(vkQueueSubmit2);
		LOAD_DEVICE_PROC(vkQueuePresentKHR);

		// Descriptor set layout
		LOAD_DEVICE_PROC(vkCreateDescriptorSetLayout);
		LOAD_DEVICE_PROC(vkDestroyDescriptorSetLayout);

		// Descriptor pool
		LOAD_DEVICE_PROC(vkCreateDescriptorPool);
		LOAD_DEVICE_PROC(vkDestroyDescriptorPool);
		LOAD_DEVICE_PROC(vkResetDescriptorPool);

		// Descriptor set
		LOAD_DEVICE_PROC(vkAllocateDescriptorSets);
		LOAD_DEVICE_PROC(vkFreeDescriptorSets);
		LOAD_DEVICE_PROC(vkUpdateDescriptorSets);

		// Shader module;
		LOAD_DEVICE_PROC(vkCreateShaderModule);
		LOAD_DEVICE_PROC(vkDestroyShaderModule);

		// Render pass
		LOAD_DEVICE_PROC(vkCreateRenderPass2);
		LOAD_DEVICE_PROC(vkDestroyRenderPass);

		// Pipeline layout
		LOAD_DEVICE_PROC(vkCreatePipelineLayout);
		LOAD_DEVICE_PROC(vkDestroyPipelineLayout);

		// Pipeline cache
		LOAD_DEVICE_PROC(vkCreatePipelineCache);
		LOAD_DEVICE_PROC(vkDestroyPipelineCache);

		// Pipeline
		LOAD_DEVICE_PROC(vkCreateGraphicsPipelines);
		LOAD_DEVICE_PROC(vkCreateComputePipelines);
		LOAD_DEVICE_PROC(vkDestroyPipeline);

		// Swapchain
		LOAD_DEVICE_PROC(vkCreateSwapchainKHR);
		LOAD_DEVICE_PROC(vkDestroySwapchainKHR);
		LOAD_DEVICE_PROC(vkGetSwapchainImagesKHR);
		LOAD_DEVICE_PROC(vkAcquireNextImage2KHR);

		// Image
		LOAD_DEVICE_PROC(vkCreateImage);
		LOAD_DEVICE_PROC(vkDestroyImage);
		LOAD_DEVICE_PROC(vkGetImageMemoryRequirements2);
		LOAD_DEVICE_PROC(vkBindImageMemory2);

		// Image view
		LOAD_DEVICE_PROC(vkCreateImageView);
		LOAD_DEVICE_PROC(vkDestroyImageView);

		// Sampler
		LOAD_DEVICE_PROC(vkCreateSampler);
		LOAD_DEVICE_PROC(vkDestroySampler);

		// Framebuffer
		LOAD_DEVICE_PROC(vkCreateFramebuffer);
		LOAD_DEVICE_PROC(vkDestroyFramebuffer);

		// Semaphore
		LOAD_DEVICE_PROC(vkCreateSemaphore);
		LOAD_DEVICE_PROC(vkDestroySemaphore);

		// Fence
		LOAD_DEVICE_PROC(vkCreateFence);
		LOAD_DEVICE_PROC(vkDestroyFence);
		LOAD_DEVICE_PROC(vkWaitForFences);
		LOAD_DEVICE_PROC(vkResetFences);

		// Command pool
		LOAD_DEVICE_PROC(vkCreateCommandPool);
		LOAD_DEVICE_PROC(vkDestroyCommandPool);
		LOAD_DEVICE_PROC(vkAllocateCommandBuffers);
		LOAD_DEVICE_PROC(vkResetCommandPool);

		// Command buffer
		LOAD_DEVICE_PROC(vkBeginCommandBuffer);
		LOAD_DEVICE_PROC(vkEndCommandBuffer);
		LOAD_DEVICE_PROC(vkCmdBeginRenderPass2);
		LOAD_DEVICE_PROC(vkCmdEndRenderPass2);
		LOAD_DEVICE_PROC(vkCmdBindPipeline);
		LOAD_DEVICE_PROC(vkCmdSetViewport);
		LOAD_DEVICE_PROC(vkCmdSetScissor);
		LOAD_DEVICE_PROC(vkCmdDraw);
		LOAD_DEVICE_PROC(vkCmdDrawIndexed);
		LOAD_DEVICE_PROC(vkCmdCopyBuffer2);
		LOAD_DEVICE_PROC(vkCmdCopyBufferToImage2);
		LOAD_DEVICE_PROC(vkCmdPipelineBarrier2);
		LOAD_DEVICE_PROC(vkCmdBindVertexBuffers);
		LOAD_DEVICE_PROC(vkCmdBindIndexBuffer);
		LOAD_DEVICE_PROC(vkCmdBindDescriptorSets);
		LOAD_DEVICE_PROC(vkCmdExecuteCommands);
		LOAD_DEVICE_PROC(vkCmdSetVertexInputEXT);

		// Memory
		LOAD_DEVICE_PROC(vkAllocateMemory);
		LOAD_DEVICE_PROC(vkFreeMemory);
		LOAD_DEVICE_PROC(vkMapMemory);
		LOAD_DEVICE_PROC(vkUnmapMemory);

		// Buffer
		LOAD_DEVICE_PROC(vkCreateBuffer);
		LOAD_DEVICE_PROC(vkDestroyBuffer);
		LOAD_DEVICE_PROC(vkGetBufferMemoryRequirements2);
		LOAD_DEVICE_PROC(vkBindBufferMemory2);
	}

	VkDevice Device::__create(
		PhysicalDevice const &physicalDevice,
		VkDeviceCreateInfo const &createInfo)
	{
		VkDevice retVal{ };
		auto const result{ physicalDevice.vkCreateDevice(&createInfo, nullptr, &retVal) };

		if (result != VkResult::VK_SUCCESS)
			throw std::runtime_error{ "Cannot create a VkDevice." };

		return retVal;
	}
}