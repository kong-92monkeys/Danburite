#pragma warning(disable: 5103)

#include "Device.h"
#include <stdexcept>

#define LOAD_DEVICE_PROC(funcName) (__deviceProc.##funcName = reinterpret_cast<PFN_##funcName>(__physicalDevice.vkGetDeviceProcAddr(getHandle(), #funcName)))

namespace VK
{
	Device::Device(
		PhysicalDevice &physicalDevice,
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
		PhysicalDevice &physicalDevice,
		VkDeviceCreateInfo const &createInfo)
	{
		VkDevice retVal{ };
		auto const result{ physicalDevice.vkCreateDevice(&createInfo, nullptr, &retVal) };

		if (result != VkResult::VK_SUCCESS)
			throw std::runtime_error{ "Cannot create a VkDevice." };

		return retVal;
	}
}