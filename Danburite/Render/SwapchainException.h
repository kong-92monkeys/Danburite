#pragma once

#include <exception>
#include "../Vulkan/Vulkan.h"

namespace Render
{
	class SwapchainException : public std::exception
	{
	public:
		SwapchainException(
			char const *_Message,
			VkResult result) noexcept;

		[[nodiscard]]
		constexpr VkResult getResult() const noexcept;

	private:
		VkResult __result{ };
	};

	constexpr VkResult SwapchainException::getResult() const noexcept
	{
		return __result;
	}
}