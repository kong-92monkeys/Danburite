#include "Surface.h"
#include <stdexcept>

namespace VK
{
	Surface::Surface(
		Instance &instance,
		VkWin32SurfaceCreateInfoKHR const &createInfo) :
		Handle		{ __create(instance, createInfo) },
		__instance	{ instance }
	{}

	Surface::~Surface() noexcept
	{
		__instance.vkDestroySurfaceKHR(getHandle(), nullptr);
	}

	VkSurfaceKHR Surface::__create(
		Instance &instance,
		VkWin32SurfaceCreateInfoKHR const &createInfo)
	{
		VkSurfaceKHR retVal{ };
		auto const result{ instance.vkCreateWin32SurfaceKHR(&createInfo, nullptr, &retVal) };

		if (result != VkResult::VK_SUCCESS)
			throw std::runtime_error{ "Cannot create a VkSurfaceKHR." };

		return retVal;
	}
}