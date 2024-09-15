#include "ResourcePool.h"

namespace Render
{
	ResourcePool::ResourcePool(
		VK::Device &device,
		Infra::DeferredDeleter &deferredDeleter,
		Dev::MemoryAllocator &memoryAllocator) noexcept :
		__device			{ device },
		__deferredDeleter	{ deferredDeleter },
		__memoryAllocator	{ memoryAllocator }
	{
		__registerHostVisibleCoherentStorageBufferPool();
	}

	std::shared_ptr<Dev::MemoryBuffer> ResourcePool::getBuffer(
		BufferType const type,
		size_t const size)
	{
		return __bufferPools.at(type)->get(size);
	}

	void ResourcePool::recycleBuffer(
		BufferType const type,
		std::shared_ptr<Dev::MemoryBuffer> &&pBuffer) noexcept
	{
		return __bufferPools.at(type)->recycle(std::move(pBuffer));
	}

	void ResourcePool::__registerHostVisibleCoherentStorageBufferPool()
	{
		auto creator = [this] (size_t const size)
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

			return std::make_shared<Dev::MemoryBuffer>(
				__device, __memoryAllocator, bufferCreateInfo, memoryProp);
		};

		__bufferPools[BufferType::HOST_VISIBLE_COHERENT_STORAGE] =
			std::make_unique<__BufferPool>(std::move(creator), __deferredDeleter);
	}

	ResourcePool::__BufferPool::__BufferPool(
		std::function<std::shared_ptr<Dev::MemoryBuffer>(size_t)> &&creator,
		Infra::DeferredDeleter &deferredDeleter) noexcept :
		__creator	{ std::move(creator) },
		__recycler	{ deferredDeleter }
	{}

	std::shared_ptr<Dev::MemoryBuffer> ResourcePool::__BufferPool::get(
		size_t const size)
	{
		auto pRetVal
		{
			__recycler.retrieveWhere([size] (auto const &elem)
			{
				return (elem.getSize() >= size);
			})
		};

		if (!pRetVal)
			pRetVal = __creator(size);

		return pRetVal;
	}

	void ResourcePool::__BufferPool::recycle(
		std::shared_ptr<Dev::MemoryBuffer> &&pBuffer) noexcept
	{
		__recycler.recycle(std::move(pBuffer));
	}
}