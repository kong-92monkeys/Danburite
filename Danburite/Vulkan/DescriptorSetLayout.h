#pragma once

#include "Device.h"

namespace VK
{
	class DescriptorSetLayout : public Infra::Handle<VkDescriptorSetLayout>
	{
	public:
		DescriptorSetLayout(
			Device &device,
			VkDescriptorSetLayoutCreateInfo const &createInfo);

		virtual ~DescriptorSetLayout() noexcept override;

	private:
		Device &__device;

		[[nodiscard]]
		static VkDescriptorSetLayout __create(
			Device &device,
			VkDescriptorSetLayoutCreateInfo const &createInfo);
	};
}