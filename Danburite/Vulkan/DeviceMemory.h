#pragma once

#include "Device.h"

namespace VK
{
	class DeviceMemory : public Infra::Handle<VkDeviceMemory>
	{
	public:
		DeviceMemory(
			Device &device,
			VkMemoryAllocateInfo const &allocInfo);

		virtual ~DeviceMemory() noexcept override;

		[[nodiscard]]
		void *getMappedMemory() noexcept;

		[[nodiscard]]
		constexpr VkDeviceSize getSize() const noexcept;

		[[nodiscard]]
		constexpr uint32_t getTypeIndex() const noexcept;

	private:
		Device &__device;

		VkDeviceSize const __size;
		uint32_t const __typeIndex;

		void *__pMapped{ };

		[[nodiscard]]
		static VkDeviceMemory __create(
			Device &device,
			VkMemoryAllocateInfo const &allocInfo);
	};

	constexpr VkDeviceSize DeviceMemory::getSize() const noexcept
	{
		return __size;
	}

	constexpr uint32_t DeviceMemory::getTypeIndex() const noexcept
	{
		return __typeIndex;
	}
}