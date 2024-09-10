#pragma once

#include "Instance.h"

namespace VK
{
	class Surface : public Infra::Handle<VkSurfaceKHR>
	{
	public:
		Surface(
			Instance &instance,
			VkWin32SurfaceCreateInfoKHR const &createInfo);

		virtual ~Surface() noexcept override;

	private:
		Instance &__instance;

		[[nodiscard]]
		static VkSurfaceKHR __create(
			Instance &instance,
			VkWin32SurfaceCreateInfoKHR const &createInfo);
	};
}