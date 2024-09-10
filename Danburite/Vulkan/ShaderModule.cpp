#include "ShaderModule.h"
#include <stdexcept>

namespace VK
{
	ShaderModule::ShaderModule(
		Device &device,
		VkShaderModuleCreateInfo const &createInfo) :
		Handle		{ __create(device, createInfo) },
		__device	{ device }
	{}

	ShaderModule::~ShaderModule() noexcept
	{
		__device.vkDestroyShaderModule(getHandle(), nullptr);
	}

	VkShaderModule ShaderModule::__create(
		Device &device,
		VkShaderModuleCreateInfo const &createInfo)
	{
		VkShaderModule retVal{ };
		auto const result{ device.vkCreateShaderModule(&createInfo, nullptr, &retVal) };

		if (result != VkResult::VK_SUCCESS)
			throw std::runtime_error{ "Cannot create a VkShaderModule." };

		return retVal;
	}
}