#include "Image.h"
#include <stdexcept>

namespace VK
{
	Image::Image(
		Device &device,
		VkImageCreateInfo const &createInfo) :
		Handle		{ __create(device, createInfo) },
		__pDevice	{ &device },
		__format	{ createInfo.format },
		__ownHandle	{ true }
	{
		__resolveMemoryRequirements();
	}

	Image::Image(
		VkImage const handle,
		VkFormat const format) noexcept :
		Handle		{ handle },
		__format	{ format },
		__ownHandle	{ false }
	{}

	Image::~Image() noexcept
	{
		if (!__ownHandle)
			return;

		__pDevice->vkDestroyImage(getHandle(), nullptr);
	}

	void Image::__resolveMemoryRequirements() noexcept
	{
		__memDedicatedReq.sType = VkStructureType::VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS;

		__memReq2.sType = VkStructureType::VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
		__memReq2.pNext = &__memDedicatedReq;

		const VkImageMemoryRequirementsInfo2 resolveInfo
		{
			.sType	{ VkStructureType::VK_STRUCTURE_TYPE_IMAGE_MEMORY_REQUIREMENTS_INFO_2 },
			.image	{ getHandle() }
		};
		
		__pDevice->vkGetImageMemoryRequirements2(&resolveInfo, &__memReq2);
	}

	VkImage Image::__create(
		Device &device,
		VkImageCreateInfo const &createInfo)
	{
		VkImage retVal{ };
		auto const result{ device.vkCreateImage(&createInfo, nullptr, &retVal) };

		if (result != VkResult::VK_SUCCESS)
			throw std::runtime_error{ "Cannot create a VkPipelineCache." };

		return retVal;
	}
}