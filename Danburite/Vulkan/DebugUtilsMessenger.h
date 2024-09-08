#pragma once

#include "Instance.h"

namespace VK
{
	class DebugUtilsMessenger : public Infra::Handle<VkDebugUtilsMessengerEXT>
	{
	public:
		explicit DebugUtilsMessenger(
			Instance &instance,
			VkDebugUtilsMessengerCreateInfoEXT const &createInfo);

		virtual ~DebugUtilsMessenger() noexcept override;

	private:
		Instance &__instance;

		[[nodiscard]]
		static VkDebugUtilsMessengerEXT __create(
			Instance &instance,
			VkDebugUtilsMessengerCreateInfoEXT const &createInfo);
	};
}