#pragma once

#include "../Device/CommandExecutor.h"
#include "../Render/ResourcePool.h"
#include <unordered_map>
#include <memory>

namespace Render
{
	class Mesh : public Infra::Unique
	{
	public:
		Mesh(
			VK::Device &device,
			Dev::MemoryAllocator &memoryAllocator,
			Dev::CommandExecutor &commandExecutor,
			Render::ResourcePool &resourcePool) noexcept;

		virtual ~Mesh() noexcept override;

		void createVertexBuffer(
			uint32_t bindingIndex,
			void const *pData,
			size_t size);

		void updateVertexBuffer(
			uint32_t bindingIndex,
			void const *pData,
			size_t size,
			size_t offset);

		void clearVertexBuffer(
			uint32_t bindingIndex);

		void clearVertexBuffers();

		void createIndexBuffer(
			VkIndexType type,
			void const *pData,
			size_t size);

		void updateIndexBuffer(
			void const *pData,
			size_t size,
			size_t offset);

		void clearIndexBuffer();

		void bind(
			VK::CommandBuffer &cmdBuffer) const;

		[[nodiscard]]
		constexpr uint32_t getVertexAttribFlags() const noexcept;

	private:
		VK::Device &__device;
		Dev::MemoryAllocator &__memoryAllocator;
		Dev::CommandExecutor &__commandExecutor;
		Render::ResourcePool &__resourcePool;

		uint32_t __vertexAttribFlags{ };
		std::unordered_map<uint32_t, std::shared_ptr<Dev::MemoryBuffer>> __vertexBuffers;

		std::vector<VkBuffer> __cmdParam_vertexBufferHandles;
		std::vector<VkDeviceSize> __cmdParam_vertexBufferOffsets;

		VkIndexType __indexType{};
		std::shared_ptr<Dev::MemoryBuffer> __pIndexBuffer;

		void __updateData(
			Dev::MemoryBuffer &dst,
			void const *pData,
			size_t size,
			size_t offset,
			VkPipelineStageFlags2 beforeStageMask,
			VkAccessFlags2 beforeAccessMask,
			VkPipelineStageFlags2 afterStageMask,
			VkAccessFlags2 afterAccessMask);

		void __validateCmdParams() noexcept;

		constexpr void __enableVertexAttribFlag(
			uint32_t bindingIndex) noexcept;

		constexpr void __disableVertexAttribFlag(
			uint32_t bindingIndex) noexcept;

		constexpr void __resetVertexAttribFlags() noexcept;
	};

	constexpr uint32_t Mesh::getVertexAttribFlags() const noexcept
	{
		return __vertexAttribFlags;
	}

	constexpr void Mesh::__enableVertexAttribFlag(
		uint32_t const bindingIndex) noexcept
	{
		__vertexAttribFlags |= (1U << bindingIndex);
	}

	constexpr void Mesh::__disableVertexAttribFlag(
		uint32_t const bindingIndex) noexcept
	{
		__vertexAttribFlags &= ~(1U << bindingIndex);
	}

	constexpr void Mesh::__resetVertexAttribFlags() noexcept
	{
		__vertexAttribFlags = 0U;
	}
}