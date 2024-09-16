#pragma once

#include "../Infra/DeferredRecycler.h"
#include "../Device/MemoryBuffer.h"

namespace Render
{
	class ResourcePool : public Infra::Unique
	{
	public:
		enum class BufferType
		{
			DEVICE_LOCAL_VERTEX,
			DEVICE_LOCAL_INDEX,
			HOST_VISIBLE_COHERENT_STORAGE,
			STAGING
		};

		ResourcePool(
			VK::Device &device,
			Infra::DeferredDeleter &deferredDeleter,
			Dev::MemoryAllocator &memoryAllocator) noexcept;

		virtual ~ResourcePool() noexcept override = default;

		[[nodiscard]]
		std::shared_ptr<Dev::MemoryBuffer> getBuffer(
			BufferType type,
			size_t size);

		void recycleBuffer(
			BufferType type,
			std::shared_ptr<Dev::MemoryBuffer> &&pBuffer) noexcept;

	private:
		class __BufferPool : public Infra::Unique
		{
		public:
			__BufferPool(
				std::function<std::shared_ptr<Dev::MemoryBuffer>(size_t)> &&creator,
				Infra::DeferredDeleter &deferredDeleter) noexcept;

			[[nodiscard]]
			std::shared_ptr<Dev::MemoryBuffer> get(
				size_t size);

			void recycle(
				std::shared_ptr<Dev::MemoryBuffer> &&pBuffer) noexcept;

		private:
			std::function<std::shared_ptr<Dev::MemoryBuffer>(size_t)> const __creator;
			Infra::DeferredRecycler<Dev::MemoryBuffer> __recycler;
		};

		VK::Device &__device;
		Infra::DeferredDeleter &__deferredDeleter;
		Dev::MemoryAllocator &__memoryAllocator;

		std::unordered_map<BufferType, std::unique_ptr<__BufferPool>> __bufferPools;

		void __registerDeviceLocalVertexBufferPool();
		void __registerDeviceLocalIndexBufferPool();
		void __registerHostVisibleCoherentStorageBufferPool();
		void __registerStagingBufferPool();

		void __registerBufferPool(
			BufferType bufferType,
			VkBufferUsageFlags bufferUsage,
			VkMemoryPropertyFlags memoryProp);
	};
}