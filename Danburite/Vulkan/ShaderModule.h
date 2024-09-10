#pragma once

#include "Device.h"

namespace VK
{
	class ShaderModule : public Infra::Handle<VkShaderModule>
	{
	public:
		ShaderModule(
			Device &device,
			VkShaderModuleCreateInfo const &createInfo);

		virtual ~ShaderModule() noexcept override;

	private:
		Device &__device;

		[[nodiscard]]
		static VkShaderModule __create(
			Device &device,
			VkShaderModuleCreateInfo const &createInfo);
	};
}