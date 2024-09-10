#pragma once

#include "Device.h"

namespace VK
{
	class Sampler : public Infra::Handle<VkSampler>
	{
	public:
		Sampler(
			Device &device,
			VkSamplerCreateInfo const &createInfo);

		virtual ~Sampler() noexcept override;

	private:
		Device &__device;

		[[nodiscard]]
		static VkSampler __create(
			Device &device,
			VkSamplerCreateInfo const &createInfo);
	};
}