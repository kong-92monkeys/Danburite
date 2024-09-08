#include "DebugUtilsMessenger.h"
#include <stdexcept>

namespace VK
{
	DebugUtilsMessenger::DebugUtilsMessenger(
		Instance &instance,
		VkDebugUtilsMessengerCreateInfoEXT const &createInfo) :
		Handle		{ __create(instance, createInfo) },
		__instance	{ instance }
	{}

	DebugUtilsMessenger::~DebugUtilsMessenger() noexcept
	{
		__instance.vkDestroyDebugUtilsMessengerEXT(getHandle(), nullptr);
	}

	VkDebugUtilsMessengerEXT DebugUtilsMessenger::__create(
		Instance &instance,
		VkDebugUtilsMessengerCreateInfoEXT const &createInfo)
	{
		VkDebugUtilsMessengerEXT retVal{ };
		auto const result{ instance.vkCreateDebugUtilsMessengerEXT(&createInfo, nullptr, &retVal) };

		if (result != VkResult::VK_SUCCESS)
			throw std::runtime_error{ "Cannot create a VkDebugUtilsMessengerEXT." };

		return retVal;
	}
}