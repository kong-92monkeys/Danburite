#pragma once

#include "../Infra/DeferredDeleter.h"
#include "../Vulkan/CommandBuffer.h"
#include "../Device/MemoryBuffer.h"
#include "../Device/CommandExecutor.h"
#include <unordered_map>
#include <memory>

namespace Render
{
	class Mesh : public Infra::Unique
	{
	public:
		Mesh(
			VK::Device &device,
			Dev::CommandExecutor &commandExecutor,
			Dev::MemoryAllocator &memoryAllocator,
			Infra::DeferredDeleter &deferredDeleter) noexcept;

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

	private:
		VK::Device &__device;
		Dev::CommandExecutor &__commandExecutor;
		Dev::MemoryAllocator &__memoryAllocator;
		Infra::DeferredDeleter &__deferredDeleter;

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
			VkPipelineStageFlags2 targetStageMask,
			VkAccessFlags2 targetAccessMask);

		void __validateCmdParams() noexcept;

		[[nodiscard]]
		std::shared_ptr<Dev::MemoryBuffer> __createStagingBuffer(
			size_t size);
	};
}