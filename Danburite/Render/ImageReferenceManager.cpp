#include "ImageReferenceManager.h"

namespace Render
{
	void ImageReferenceManager::addImage(
		VK::ImageView const *const pImage)
	{
		auto &[ref, id] { __refIdMap[pImage] };
		if (!ref)
		{
			id = __idAllocator.allocate();
			__registerImage(pImage);
		}

		++ref;
	}

	void ImageReferenceManager::removeImage(
		VK::ImageView const *const pImage)
	{
		auto &[ref, id] { __refIdMap[pImage] };
		--ref;

		if (!ref)
		{
			__unregisterImage(pImage);
			__idAllocator.free(id);
		}
	}

	uint32_t ImageReferenceManager::getIdOf(
		VK::ImageView const *const pImage) const noexcept
	{
		return __refIdMap.at(pImage).second;
	}

	void ImageReferenceManager::__registerImage(
		VK::ImageView const *const pImage)
	{
		__updateDescInfo(pImage);
		_invokeUpdateEvent();
	}

	void ImageReferenceManager::__unregisterImage(
		VK::ImageView const *const pImage)
	{
		__refIdMap.erase(pImage);
	}

	void ImageReferenceManager::__updateDescInfo(
		VK::ImageView const *pImage) noexcept
	{
		size_t const imageId{ __refIdMap.at(pImage).second };
		
		if (__descInfos.size() <= imageId)
			__descInfos.resize(imageId + 1ULL);

		auto &info{ __descInfos[imageId] };
		info.sampler		= VK_NULL_HANDLE;
		info.imageView		= pImage->getHandle();
		info.imageLayout	= VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	}
}