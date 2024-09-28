#pragma once

#include "Device.h"

namespace VK
{
	class CommandBuffer : public Infra::Handle<VkCommandBuffer>
	{
	public:
		CommandBuffer(
			Device &device,
			VkCommandBuffer handle) noexcept;

		VkResult vkBeginCommandBuffer(
			VkCommandBufferBeginInfo const *pBeginInfo);

		VkResult vkEndCommandBuffer();

		void vkCmdBeginRenderPass2(
			VkRenderPassBeginInfo const *pRenderPassBegin,
			VkSubpassBeginInfo const *pSubpassBeginInfo);

		void vkCmdEndRenderPass2(
			VkSubpassEndInfo const *pSubpassEndInfo);

		void vkCmdBindPipeline(
			VkPipelineBindPoint pipelineBindPoint,
			VkPipeline pipeline);

		void vkCmdSetViewport(
			uint32_t firstViewport,
			uint32_t viewportCount,
			VkViewport const *pViewports);

		void vkCmdSetScissor(
			uint32_t firstScissor,
			uint32_t scissorCount,
			VkRect2D const *pScissors);

		void vkCmdDraw(
			uint32_t vertexCount,
			uint32_t instanceCount,
			uint32_t firstVertex,
			uint32_t firstInstance);

		void vkCmdDrawIndexed(
			uint32_t indexCount,
			uint32_t instanceCount,
			uint32_t firstIndex,
			int32_t vertexOffset,
			uint32_t firstInstance);

		void vkCmdCopyBuffer2(
			VkCopyBufferInfo2 const *pCopyBufferInfo);

		void vkCmdCopyBufferToImage2(
			VkCopyBufferToImageInfo2 const *pCopyBufferToImageInfo);

		void vkCmdPipelineBarrier2(
			VkDependencyInfo const *pDependencyInfo);

		void vkCmdBindVertexBuffers(
			uint32_t firstBinding,
			uint32_t bindingCount,
			VkBuffer const *pBuffers,
			VkDeviceSize const *pOffsets);

		void vkCmdBindIndexBuffer(
			VkBuffer buffer,
			VkDeviceSize offset,
			VkIndexType indexType);

		void vkCmdBindDescriptorSets(
			VkPipelineBindPoint pipelineBindPoint,
			VkPipelineLayout layout,
			uint32_t firstSet,
			uint32_t descriptorSetCount,
			VkDescriptorSet const *pDescriptorSets,
			uint32_t dynamicOffsetCount,
			uint32_t const *pDynamicOffsets);

		void vkCmdExecuteCommands(
			uint32_t commandBufferCount,
			VkCommandBuffer const *pCommandBuffers);

		void vkCmdPushConstants(
			VkPipelineLayout layout,
			VkShaderStageFlags stageFlags,
			uint32_t offset,
			uint32_t size,
			void const *pValues);

		void vkCmdSetVertexInputEXT(
			uint32_t vertexBindingDescriptionCount,
			VkVertexInputBindingDescription2EXT const *pVertexBindingDescriptions,
			uint32_t vertexAttributeDescriptionCount,
			VkVertexInputAttributeDescription2EXT const *pVertexAttributeDescriptions);

	private:
		Device &__device;
	};
}