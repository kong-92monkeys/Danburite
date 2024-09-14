#include "LayerResourcePool.h"

namespace Render
{
	LayerResourcePool::LayerResourcePool(
		VK::Device &device,
		Infra::DeferredDeleter &deferredDeleter,
		Dev::MemoryAllocator &memoryAllocator) noexcept :
		__device					{ device },
		__memoryAllocator			{ memoryAllocator },
		__storageBufferRecycler		{ deferredDeleter }
	{}

	std::shared_ptr<Dev::MemoryBuffer> LayerResourcePool::getStorageBuffer(
		size_t const size)
	{
		auto pRetVal
		{
			__storageBufferRecycler.retrieveWhere([size] (const auto &elem)
			{
				return (elem.getSize() >= size);
			})
		};

		if (!pRetVal)
		{
			VkBufferCreateInfo const bufferCreateInfo
			{
				.sType			{ VkStructureType::VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO },
				.size			{ size },
				.usage			{ VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT },
				.sharingMode	{ VkSharingMode::VK_SHARING_MODE_EXCLUSIVE }
			};

			VkMemoryPropertyFlags memoryProp{ };
			memoryProp |= VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
			memoryProp |= VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

			pRetVal = std::make_shared<Dev::MemoryBuffer>(
				__device, __memoryAllocator, bufferCreateInfo, memoryProp);
		}

		return pRetVal;
	}

	void LayerResourcePool::recycleStorageBuffer(
		std::shared_ptr<Dev::MemoryBuffer> &&pBuffer) noexcept
	{
		__storageBufferRecycler.recycle(std::move(pBuffer));
	}
}