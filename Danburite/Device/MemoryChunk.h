#pragma once

#include "../Infra/RegionAllocator.h"
#include "../Vulkan/DeviceMemory.h"
#include <memory>

namespace Dev
{
	class MemoryChunk : public Infra::Unique
	{
	public:
		[[nodiscard]]
		virtual VK::DeviceMemory &getMemory() noexcept = 0;

		[[nodiscard]]
		virtual size_t getSize() const noexcept = 0;

		[[nodiscard]]
		virtual size_t getOffset() const noexcept = 0;
	};

	class MemoryChunk_Dedicated : public MemoryChunk
	{
	public:
		MemoryChunk_Dedicated(
			std::unique_ptr<VK::DeviceMemory> &&pMemory) noexcept;

		[[nodiscard]]
		virtual VK::DeviceMemory &getMemory() noexcept override;

		[[nodiscard]]
		virtual size_t getSize() const noexcept override;

		[[nodiscard]]
		virtual size_t getOffset() const noexcept override;

	private:
		std::unique_ptr<VK::DeviceMemory> __pMemory;
	};

	class MemoryChunk_SubAllocated : public MemoryChunk
	{
	public:
		MemoryChunk_SubAllocated(
			VK::DeviceMemory &memory,
			std::unique_ptr<Infra::Region> &&pRegion);

		[[nodiscard]]
		virtual VK::DeviceMemory &getMemory() noexcept override;

		[[nodiscard]]
		virtual size_t getSize() const noexcept override;

		[[nodiscard]]
		virtual size_t getOffset() const noexcept override;

	private:
		VK::DeviceMemory &__memory;
		std::unique_ptr<Infra::Region> const __pRegion;
	};
}