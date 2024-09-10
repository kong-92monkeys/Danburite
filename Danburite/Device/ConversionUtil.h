#pragma once

#include "../Infra/Version.h"
#include "../Vulkan/Vulkan.h"

namespace Dev::ConversionUtil
{
	constexpr Infra::Version fromVulkanVersion(const uint32_t version) noexcept
	{
		return
		{
			.major		{ VK_API_VERSION_MAJOR(version) },
			.minor		{ VK_API_VERSION_MINOR(version) },
			.patch		{ VK_API_VERSION_PATCH(version) },
			.variant	{ VK_API_VERSION_VARIANT(version) }
		};
	}

	constexpr uint32_t toVulkanVersion(const Infra::Version &version) noexcept
	{
		return VK_MAKE_API_VERSION(version.variant, version.major, version.minor, version.patch);
	}
}