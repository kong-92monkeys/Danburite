#include "CommandBuffer.h"

namespace VK
{
	CommandBuffer::CommandBuffer(
		Device &device,
		VkCommandBuffer const handle) noexcept :
		Handle		{ handle },
		__device	{ device }
	{}

	VkResult CommandBuffer::vkBeginCommandBuffer(
		VkCommandBufferBeginInfo const *const pBeginInfo)
	{
		return __device.vkBeginCommandBuffer(
			getHandle(), pBeginInfo);
	}

	VkResult CommandBuffer::vkEndCommandBuffer()
	{
		return __device.vkEndCommandBuffer(
			getHandle());
	}

	void CommandBuffer::vkCmdBeginRenderPass2(
		VkRenderPassBeginInfo const *const pRenderPassBegin,
		VkSubpassBeginInfo const *const pSubpassBeginInfo)
	{
		__device.vkCmdBeginRenderPass2(
			getHandle(), pRenderPassBegin, pSubpassBeginInfo);
	}

	void CommandBuffer::vkCmdEndRenderPass2(
		VkSubpassEndInfo const *const pSubpassEndInfo)
	{
		__device.vkCmdEndRenderPass2(
			getHandle(), pSubpassEndInfo);
	}

	void CommandBuffer::vkCmdBindPipeline(
		VkPipelineBindPoint const pipelineBindPoint,
		VkPipeline const pipeline)
	{
		__device.vkCmdBindPipeline(
			getHandle(), pipelineBindPoint, pipeline);
	}

	void CommandBuffer::vkCmdSetViewport(
		uint32_t const firstViewport,
		uint32_t const viewportCount,
		VkViewport const *const pViewports)
	{
		__device.vkCmdSetViewport(
			getHandle(), firstViewport, viewportCount, pViewports);
	}

	void CommandBuffer::vkCmdSetScissor(
		uint32_t const firstScissor,
		uint32_t const scissorCount,
		VkRect2D const *const pScissors)
	{
		__device.vkCmdSetScissor(
			getHandle(), firstScissor, scissorCount, pScissors);
	}

	void CommandBuffer::vkCmdDraw(
		uint32_t const vertexCount,
		uint32_t const instanceCount,
		uint32_t const firstVertex,
		uint32_t const firstInstance)
	{
		__device.vkCmdDraw(
			getHandle(), vertexCount, instanceCount, firstVertex, firstInstance);
	}

	void CommandBuffer::vkCmdDrawIndexed(
		uint32_t const indexCount,
		uint32_t const instanceCount,
		uint32_t const firstIndex,
		int32_t const vertexOffset,
		uint32_t const firstInstance)
	{
		__device.vkCmdDrawIndexed(
			getHandle(), indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
	}

	void CommandBuffer::vkCmdCopyBuffer2(
		VkCopyBufferInfo2 const *const pCopyBufferInfo)
	{
		__device.vkCmdCopyBuffer2(
			getHandle(), pCopyBufferInfo);
	}

	void CommandBuffer::vkCmdCopyBufferToImage2(
		VkCopyBufferToImageInfo2 const *const pCopyBufferToImageInfo)
	{
		__device.vkCmdCopyBufferToImage2(
			getHandle(), pCopyBufferToImageInfo);
	}

	void CommandBuffer::vkCmdPipelineBarrier2(
		VkDependencyInfo const *const pDependencyInfo)
	{
		__device.vkCmdPipelineBarrier2(
			getHandle(), pDependencyInfo);
	}

	void CommandBuffer::vkCmdBindVertexBuffers(
		uint32_t const firstBinding,
		uint32_t const bindingCount,
		VkBuffer const *const pBuffers,
		VkDeviceSize const *const pOffsets)
	{
		__device.vkCmdBindVertexBuffers(
			getHandle(), firstBinding, bindingCount, pBuffers, pOffsets);
	}

	void CommandBuffer::vkCmdBindIndexBuffer(
		VkBuffer const buffer,
		VkDeviceSize const offset,
		VkIndexType const indexType)
	{
		__device.vkCmdBindIndexBuffer(
			getHandle(), buffer, offset, indexType);
	}

	void CommandBuffer::vkCmdBindDescriptorSets(
		VkPipelineBindPoint const pipelineBindPoint,
		VkPipelineLayout const layout,
		uint32_t const firstSet,
		uint32_t const descriptorSetCount,
		VkDescriptorSet const *const pDescriptorSets,
		uint32_t const dynamicOffsetCount,
		uint32_t const *const pDynamicOffsets)
	{
		__device.vkCmdBindDescriptorSets(
			getHandle(), pipelineBindPoint, layout,
			firstSet, descriptorSetCount, pDescriptorSets,
			dynamicOffsetCount, pDynamicOffsets);
	}

	void CommandBuffer::vkCmdExecuteCommands(
		uint32_t const commandBufferCount,
		VkCommandBuffer const *const pCommandBuffers)
	{
		__device.vkCmdExecuteCommands(
			getHandle(), commandBufferCount, pCommandBuffers);
	}

	void CommandBuffer::vkCmdPushConstants(
		VkPipelineLayout const layout,
		VkShaderStageFlags const stageFlags,
		uint32_t const offset,
		uint32_t const size,
		void const *const pValues)
	{
		__device.vkCmdPushConstants(
			getHandle(), layout, stageFlags, offset, size, pValues);
	}

	void CommandBuffer::vkCmdSetVertexInputEXT(
		uint32_t const vertexBindingDescriptionCount,
		VkVertexInputBindingDescription2EXT const *const pVertexBindingDescriptions,
		uint32_t const vertexAttributeDescriptionCount,
		VkVertexInputAttributeDescription2EXT const *const pVertexAttributeDescriptions)
	{
		__device.vkCmdSetVertexInputEXT(
			getHandle(),
			vertexBindingDescriptionCount, pVertexBindingDescriptions,
			vertexAttributeDescriptionCount, pVertexAttributeDescriptions);
	}
}