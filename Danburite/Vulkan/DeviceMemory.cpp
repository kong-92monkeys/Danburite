#include "DeviceMemory.h"
#include <stdexcept>

namespace VK
{
	DeviceMemory::DeviceMemory(
		Device &device,
		VkMemoryAllocateInfo const &allocInfo) :
		Handle			{ __create(device, allocInfo) },
		__device		{ device },
		__size			{ allocInfo.allocationSize },
		__typeIndex		{ allocInfo.memoryTypeIndex }
	{}

	DeviceMemory::~DeviceMemory() noexcept
	{
		if (__pMapped)
			__device.vkUnmapMemory(getHandle());

		__device.vkFreeMemory(getHandle(), nullptr);
	}

	void *DeviceMemory::getMappedMemory() noexcept
	{
		if (!__pMapped)
			__device.vkMapMemory(getHandle(), 0U, VK_WHOLE_SIZE, 0U, &__pMapped);

		return __pMapped;
	}

	VkDeviceMemory DeviceMemory::__create(
		Device &device,
		VkMemoryAllocateInfo const &allocInfo)
	{
		VkDeviceMemory retVal{ };
		auto const result{ device.vkAllocateMemory(&allocInfo, nullptr, &retVal) };

		if (result != VkResult::VK_SUCCESS)
			throw std::runtime_error{ "Cannot allocate a VkDeviceMemory." };

		return retVal;
	}
}
