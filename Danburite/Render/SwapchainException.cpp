#include "SwapchainException.h"

namespace Render
{
	SwapchainException::SwapchainException(
		char const *const _Message,
		VkResult const result) noexcept :
		exception{ _Message }, __result{ result }
	{}
}