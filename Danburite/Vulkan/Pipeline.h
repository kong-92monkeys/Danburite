#pragma once

#include "Device.h"

namespace VK
{
	class Pipeline : public Infra::Handle<VkPipeline>
	{
	public:
		Pipeline(
			Device &device,
			VkPipelineCache hPipelineCache,
			VkGraphicsPipelineCreateInfo const &createInfo);

		Pipeline(
			Device &device,
			VkPipelineCache hPipelineCache,
			VkComputePipelineCreateInfo const &createInfo);

		virtual ~Pipeline() noexcept override;

	private:
		Device &__device;

		[[nodiscard]]
		static VkPipeline __create(
			Device &device,
			VkPipelineCache hPipelineCache,
			VkGraphicsPipelineCreateInfo const &createInfo);

		[[nodiscard]]
		static VkPipeline __create(
			Device &device,
			VkPipelineCache hPipelineCache,
			VkComputePipelineCreateInfo const &createInfo);
	};
}