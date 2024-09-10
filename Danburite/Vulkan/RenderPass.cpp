#include "RenderPass.h"
#include <stdexcept>

namespace VK
{
	RenderPass::RenderPass(
		Device &device,
		VkRenderPassCreateInfo2 const &createInfo) :
		Handle		{ __create(device, createInfo) },
		__device	{ device }
	{}

	RenderPass::~RenderPass() noexcept
	{
		__device.vkDestroyRenderPass(getHandle(), nullptr);
	}

	VkRenderPass RenderPass::__create(
		Device &device,
		VkRenderPassCreateInfo2 const &createInfo)
	{
		VkRenderPass retVal{ };
		auto const result{ device.vkCreateRenderPass2(&createInfo, nullptr, &retVal) };

		if (result != VkResult::VK_SUCCESS)
			throw std::runtime_error{ "Cannot create a VkRenderPass." };

		return retVal;
	}
}