#pragma once

#include "Device.h"

namespace VK
{
	class PipelineLayout : public Infra::Handle<VkPipelineLayout>
	{
	public:
		PipelineLayout(
			Device &device,
			VkPipelineLayoutCreateInfo const &createInfo);

		virtual ~PipelineLayout() noexcept override;

	private:
		Device &__device;

		[[nodiscard]]
		static VkPipelineLayout __create(
			Device &device,
			VkPipelineLayoutCreateInfo const &createInfo);
	};
}