#include "MemoryAllocator.h"

namespace Dev
{
	MemoryAllocator::MemoryAllocator(
		VK::PhysicalDevice const &physicalDevice,
		VK::Device &device,
		size_t const memoryBlockSize,
		VkDeviceSize const uniformBufferAlignment,
		VkDeviceSize const storageBufferAlignment) noexcept :
		__physicalDevice			{ physicalDevice },
		__device					{ device },
		__memoryBlockSize			{ memoryBlockSize },
		__uniformBufferAlignment	{ uniformBufferAlignment },
		__storageBufferAlignment	{ storageBufferAlignment }
	{}

	std::unique_ptr<MemoryChunk> MemoryAllocator::allocateBufferMemory(
		VK::Buffer &buffer,
		VkMemoryPropertyFlags const memoryProp)
	{
		auto const &memReq			{ buffer.getMemoryRequirements() };
		auto const memoryTypeIndex	{ resolveMemoryTypeIndex(memReq, memoryProp) };

		if (!(memoryTypeIndex.has_value()))
			return nullptr;

		auto const &memDedicatedReq{ buffer.getMemoryDedicatedRequirements() };
		//if (memDedicatedReq.prefersDedicatedAllocation || memDedicatedReq.requiresDedicatedAllocation)
		{
			VkMemoryAllocateInfo const memoryAllocInfo
			{
				.sType				{ VkStructureType::VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO },
				.allocationSize		{ memReq.size },
				.memoryTypeIndex	{ memoryTypeIndex.value() }
			};

			return std::make_unique<MemoryChunk_Dedicated>(
				std::make_unique<VK::DeviceMemory>(__device, memoryAllocInfo));
		}

		auto const alignment	{ __resolveBufferAlignment(memReq.alignment, buffer.getUsage()) };
		auto &memoryBlocks		{ __bufferMemoryBlockMap[memoryTypeIndex.value()] };

		for (auto const &pMemoryBlock : memoryBlocks)
		{
			try
			{
				return std::make_unique<MemoryChunk_SubAllocated>(
					pMemoryBlock->getMemory(),
					pMemoryBlock->allocateRegion(memReq.size, alignment));
			}
			catch(...)
			{ }
		}

		size_t const newMemoryBlockSize{ std::max(__memoryBlockSize, memReq.size) };

		auto pNewMemoryBlock
		{
			std::make_unique<MemoryBlock>(
				__device, newMemoryBlockSize, memoryTypeIndex.value())
		};

		auto pRetVal
		{
			std::make_unique<MemoryChunk_SubAllocated>(
				pNewMemoryBlock->getMemory(),
				pNewMemoryBlock->allocateRegion(memReq.size, alignment))
		};

		memoryBlocks.emplace_back(std::move(pNewMemoryBlock));
		return pRetVal;
	}

	std::unique_ptr<MemoryChunk> MemoryAllocator::allocateImageMemory(
		VK::Image &image,
		VkMemoryPropertyFlags const memoryProp)
	{
		auto const &memReq			{ image.getMemoryRequirements() };
		auto const memoryTypeIndex	{ resolveMemoryTypeIndex(memReq, memoryProp) };

		if (!(memoryTypeIndex.has_value()))
			return nullptr;

		auto const &memDedicatedReq{ image.getMemoryDedicatedRequirements() };
		if (memDedicatedReq.prefersDedicatedAllocation || memDedicatedReq.requiresDedicatedAllocation)
		{
			VkMemoryAllocateInfo const memoryAllocInfo
			{
				.sType				{ VkStructureType::VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO },
				.allocationSize		{ memReq.size },
				.memoryTypeIndex	{ memoryTypeIndex.value() }
			};

			return std::make_unique<MemoryChunk_Dedicated>(
				std::make_unique<VK::DeviceMemory>(__device, memoryAllocInfo));
		}

		auto &memoryBlocks{ __imageMemoryBlockMap[memoryTypeIndex.value()] };
		for (auto const &pMemoryBlock : memoryBlocks)
		{
			try
			{
				return std::make_unique<MemoryChunk_SubAllocated>(
					pMemoryBlock->getMemory(),
					pMemoryBlock->allocateRegion(memReq.size, memReq.alignment));
			}
			catch(...)
			{ }
		}

		size_t const newMemoryBlockSize{ std::max(__memoryBlockSize, memReq.size) };

		auto pNewMemoryBlock
		{
			std::make_unique<MemoryBlock>(
				__device, newMemoryBlockSize, memoryTypeIndex.value())
		};

		auto pRetVal
		{
			std::make_unique<MemoryChunk_SubAllocated>(
				pNewMemoryBlock->getMemory(),
				pNewMemoryBlock->allocateRegion(memReq.size, memReq.alignment))
		};

		memoryBlocks.emplace_back(std::move(pNewMemoryBlock));
		return pRetVal;
	}

	std::optional<uint32_t> MemoryAllocator::resolveMemoryTypeIndex(
		VkMemoryRequirements const &memReq,
		VkMemoryPropertyFlags const memoryProp) const noexcept
	{
		auto const &memoryTypes{ __physicalDevice.getMemoryProps().memoryTypes };

		uint32_t memoryTypeIter{ };
		for (uint32_t memoryTypeBits{ memReq.memoryTypeBits }; memoryTypeBits; memoryTypeBits >>= 1U)
		{
			if (memoryTypeBits % 2U)
			{
				auto const &memoryType{ memoryTypes[memoryTypeIter] };
				if ((memoryType.propertyFlags & memoryProp) == memoryProp)
					return memoryTypeIter;
			}

			++memoryTypeIter;
		}

		return std::nullopt;
	}
}