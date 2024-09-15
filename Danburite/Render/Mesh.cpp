#include "Mesh.h"

namespace Render
{
	Mesh::Mesh(
		VK::Device &device,
		Dev::CommandExecutor &commandExecutor,
		Dev::MemoryAllocator &memoryAllocator,
		Infra::DeferredDeleter &deferredDeleter) noexcept :
		__device			{ device },
		__commandExecutor	{ commandExecutor },
		__memoryAllocator	{ memoryAllocator },
		__deferredDeleter	{ deferredDeleter }
	{}

	Mesh::~Mesh() noexcept
	{
		for (auto &[_, pVertexBuffer] : __vertexBuffers)
		{
			if (pVertexBuffer)
				__deferredDeleter.reserve(std::move(pVertexBuffer));
		}

		if (__pIndexBuffer)
			__deferredDeleter.reserve(std::move(__pIndexBuffer));
	}

	void Mesh::createVertexBuffer(
		uint32_t const bindingIndex,
		void const *const pData,
		size_t const size)
	{
		auto &pVertexBuffer{ __vertexBuffers[bindingIndex] };
		if (pVertexBuffer)
			__deferredDeleter.reserve(std::move(pVertexBuffer));

		VkBufferCreateInfo const bufferCreateInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO },
			.size			{ size },
			.usage			{
				VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
				VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT
			},
			.sharingMode	{ VkSharingMode::VK_SHARING_MODE_EXCLUSIVE }
		};

		pVertexBuffer = std::make_shared<Dev::MemoryBuffer>(
			__device, __memoryAllocator, bufferCreateInfo,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		__updateData(
			*pVertexBuffer, pData, size, 0ULL,
			VK_PIPELINE_STAGE_2_NONE,
			VK_ACCESS_2_NONE,
			VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT,
			VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT);

		__validateCmdParams();
	}

	void Mesh::updateVertexBuffer(
		uint32_t const bindingIndex,
		void const *const pData,
		size_t const size,
		size_t const offset)
	{
		__updateData(
			*(__vertexBuffers[bindingIndex]), pData, size, offset,
			VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT,
			VK_ACCESS_2_NONE,
			VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT,
			VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT);
	}

	void Mesh::clearVertexBuffer(
		uint32_t const bindingIndex)
	{
		auto &pVertexBuffer{ __vertexBuffers[bindingIndex] };
		if (!pVertexBuffer)
			return;

		__deferredDeleter.reserve(std::move(pVertexBuffer));
		__validateCmdParams();
	}

	void Mesh::clearVertexBuffers()
	{
		for (auto &[bindingIndex, pVertexBuffer] : __vertexBuffers)
		{
			if (!pVertexBuffer)
				continue;

			__deferredDeleter.reserve(std::move(pVertexBuffer));
		}
	}

	void Mesh::createIndexBuffer(
		VkIndexType const type,
		void const *const pData,
		size_t const size)
	{
		__indexType = type;

		if (__pIndexBuffer)
			__deferredDeleter.reserve(std::move(__pIndexBuffer));

		VkBufferCreateInfo const bufferCreateInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO },
			.size			{ size },
			.usage			{
				VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
				VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT
			},
			.sharingMode	{ VkSharingMode::VK_SHARING_MODE_EXCLUSIVE }
		};

		__pIndexBuffer = std::make_shared<Dev::MemoryBuffer>(
			__device, __memoryAllocator, bufferCreateInfo,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		__updateData(
			*__pIndexBuffer, pData, size, 0ULL,
			VK_PIPELINE_STAGE_2_NONE,
			VK_ACCESS_2_NONE,
			VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT,
			VK_ACCESS_2_INDEX_READ_BIT);
	}

	void Mesh::updateIndexBuffer(
		void const *const pData,
		size_t const size,
		size_t const offset)
	{
		__updateData(
			*__pIndexBuffer, pData, size, offset,
			VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT,
			VK_ACCESS_2_NONE,
			VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT,
			VK_ACCESS_2_INDEX_READ_BIT);
	}

	void Mesh::clearIndexBuffer()
	{
		if (!__pIndexBuffer)
			return;

		__deferredDeleter.reserve(std::move(__pIndexBuffer));
	}

	void Mesh::bind(
		VK::CommandBuffer &cmdBuffer) const
	{
		cmdBuffer.vkCmdBindVertexBuffers(
			0U,
			static_cast<uint32_t>(__cmdParam_vertexBufferHandles.size()),
			__cmdParam_vertexBufferHandles.data(),
			__cmdParam_vertexBufferOffsets.data());

		if (__pIndexBuffer)
		{
			cmdBuffer.vkCmdBindIndexBuffer(
				__pIndexBuffer->getHandle(), 0U, __indexType);
		}
	}

	void Mesh::__updateData(
		Dev::MemoryBuffer &dst,
		void const *const pData,
		size_t const size,
		size_t const offset,
		VkPipelineStageFlags2 const beforeStageMask,
		VkAccessFlags2 const beforeAccessMask,
		VkPipelineStageFlags2 const afterStageMask,
		VkAccessFlags2 const afterAccessMask)
	{
		auto pStagingBuffer{ __createStagingBuffer(size) };
		std::memcpy(pStagingBuffer->getData(), pData, size);

		__commandExecutor.reserve([=, &dst, &src{ *pStagingBuffer }](auto &cmdBuffer)
		{
			VkMemoryBarrier2 const beforeMemoryBarrier
			{
				.sType				{ VkStructureType::VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 },
				.srcStageMask		{ beforeStageMask },
				.srcAccessMask		{ beforeAccessMask },
				.dstStageMask		{ VK_PIPELINE_STAGE_2_COPY_BIT },
				.dstAccessMask		{ VK_ACCESS_2_TRANSFER_WRITE_BIT }
			};

			VkDependencyInfo const beforeBarrier
			{
				.sType				{ VkStructureType::VK_STRUCTURE_TYPE_DEPENDENCY_INFO },
				.memoryBarrierCount	{ 1U },
				.pMemoryBarriers	{ &beforeMemoryBarrier },
			};

			VkBufferCopy2 const region
			{
				.sType			{ VkStructureType::VK_STRUCTURE_TYPE_BUFFER_COPY_2 },
				.srcOffset		{ 0ULL },
				.dstOffset		{ offset },
				.size			{ size }
			};

			VkCopyBufferInfo2 const copyInfo
			{
				.sType			{ VkStructureType::VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2 },
				.srcBuffer		{ src.getHandle() },
				.dstBuffer		{ dst.getHandle() },
				.regionCount	{ 1U },
				.pRegions		{ &region }
			};

			VkMemoryBarrier2 const afterMemoryBarrier
			{
				.sType				{ VkStructureType::VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 },
				.srcStageMask		{ VK_PIPELINE_STAGE_2_COPY_BIT },
				.srcAccessMask		{ VK_ACCESS_2_TRANSFER_WRITE_BIT },
				.dstStageMask		{ afterStageMask },
				.dstAccessMask		{ afterAccessMask }
			};

			VkDependencyInfo const afterBarrier
			{
				.sType				{ VkStructureType::VK_STRUCTURE_TYPE_DEPENDENCY_INFO },
				.memoryBarrierCount	{ 1U },
				.pMemoryBarriers	{ &afterMemoryBarrier },
			};

			cmdBuffer.vkCmdPipelineBarrier2(&beforeBarrier);
			cmdBuffer.vkCmdCopyBuffer2(&copyInfo);
			cmdBuffer.vkCmdPipelineBarrier2(&afterBarrier);
		});

		__deferredDeleter.reserve(std::move(pStagingBuffer));
	}

	void Mesh::__validateCmdParams() noexcept
	{
		__cmdParam_vertexBufferHandles.clear();
		__cmdParam_vertexBufferOffsets.clear();

		for (auto const &[bindingIndex, pVertexBuffer] : __vertexBuffers)
		{
			if (!pVertexBuffer)
				continue;

			if (bindingIndex >= __cmdParam_vertexBufferHandles.size())
			{
				__cmdParam_vertexBufferHandles.resize(bindingIndex + 1U);
				__cmdParam_vertexBufferOffsets.resize(bindingIndex + 1U);
			}

			__cmdParam_vertexBufferHandles[bindingIndex] = pVertexBuffer->getHandle();
			__cmdParam_vertexBufferOffsets[bindingIndex] = 0ULL;
		}
	}

	std::shared_ptr<Dev::MemoryBuffer> Mesh::__createStagingBuffer(
		size_t const size)
	{
		VkBufferCreateInfo const bufferCreateInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO },
			.size			{ size },
			.usage			{ VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT },
			.sharingMode	{ VkSharingMode::VK_SHARING_MODE_EXCLUSIVE }
		};

		return std::make_shared<Dev::MemoryBuffer>(
			__device, __memoryAllocator, bufferCreateInfo,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	}
}