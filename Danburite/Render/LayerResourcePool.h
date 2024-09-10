#pragma once

#include "../Infra/LazyRecycler.h"
#include "../Device/MemoryBuffer.h"

namespace Render
{
	class LayerResourcePool : public Infra::Unique
	{
	public:
		LayerResourcePool(
			VK::Device &device,
			Infra::LazyDeleter &lazyDeleter,
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
		Infra::LazyRecycler<Dev::MemoryBuffer> __storageBufferRecycler;
	};
}