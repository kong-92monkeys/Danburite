#pragma once

#include "Device.h"

namespace VK
{
	class RenderPass : public Infra::Handle<VkRenderPass>
	{
	public:
		RenderPass(
			Device &device,
			VkRenderPassCreateInfo2 const &createInfo);

		virtual ~RenderPass() noexcept override;

	private:
		Device &__device;

		[[nodiscard]]
		static VkRenderPass __create(
			Device &device,
			VkRenderPassCreateInfo2 const &createInfo);
	};
}