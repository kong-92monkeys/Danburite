#pragma once

#include "Instance.h"

namespace VK
{
	class Surface : public Infra::Handle<VkSurfaceKHR>
	{
	public:
		Surface(
			Instance const &instance,
			VkWin32SurfaceCreateInfoKHR const &createInfo);

		virtual ~Surface() noexcept override;

	private:
		Instance const &__instance;

		[[nodiscard]]
		static VkSurfaceKHR __create(
			Instance const &instance,
			VkWin32SurfaceCreateInfoKHR const &createInfo);
	};
}