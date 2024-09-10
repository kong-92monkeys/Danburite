#include "Semaphore.h"
#include <stdexcept>

namespace VK
{
	Semaphore::Semaphore(
		Device &device,
		VkSemaphoreCreateInfo const &createInfo) :
		Handle		{ __create(device, createInfo) },
		__device	{ device }
	{}

	Semaphore::~Semaphore() noexcept
	{
		__device.vkDestroySemaphore(getHandle(), nullptr);
	}

	VkSemaphore Semaphore::__create(
		Device &device,
		VkSemaphoreCreateInfo const &createInfo)
	{
		VkSemaphore retVal{ };
		auto const result{ device.vkCreateSemaphore(&createInfo, nullptr, &retVal) };

		if (result != VkResult::VK_SUCCESS)
			throw std::runtime_error{ "Cannot create a VkSemaphore." };

		return retVal;
	}
}