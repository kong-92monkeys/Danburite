#pragma once

#include "Device.h"

namespace VK
{
	class PipelineCache : public Infra::Handle<VkPipelineCache>
	{
	public:
		PipelineCache(
			Device &device,
			VkPipelineCacheCreateInfo const &createInfo);

		virtual ~PipelineCache() noexcept override;

	private:
		Device &__device;

		[[nodiscard]]
		static VkPipelineCache __create(
			Device &device,
			VkPipelineCacheCreateInfo const &createInfo);
	};
}