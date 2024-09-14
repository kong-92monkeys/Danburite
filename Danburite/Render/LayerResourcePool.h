#pragma once

#include "../Infra/DeferredRecycler.h"
#include "../Device/MemoryBuffer.h"

namespace Render
{
	class LayerResourcePool : public Infra::Unique
	{
	public:
		LayerResourcePool(
			VK::Device &device,
			Infra::DeferredDeleter &deferredDeleter,
			Dev::MemoryAllocator &memoryAllocator) noexcept;

		virtual ~LayerResourcePool() noexcept override = default;

		[[nodiscard]]
		std::shared_ptr<Dev::MemoryBuffer> getStorageBuffer(
			size_t size);

		void recycleStorageBuffer(
			std::shared_ptr<Dev::MemoryBuffer> &&pBuffer) noexcept;

	private:
		VK::Device &__device;
		Dev::MemoryAllocator &__memoryAllocator;
		Infra::DeferredRecycler<Dev::MemoryBuffer> __storageBufferRecycler;
	};
}