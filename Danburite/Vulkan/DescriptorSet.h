#pragma once

#include "Device.h"

namespace VK
{
	class DescriptorSet : public Infra::Handle<VkDescriptorSet>
	{
	public:
		DescriptorSet(
			Device &device,
			bool needFree,
			VkDescriptorSetAllocateInfo const &allocInfo);

		virtual ~DescriptorSet() noexcept override;

	private:
		Device &__device;

		VkDescriptorPool const __hDescPool;
		bool const __needFree;

		[[nodiscard]]
		static VkDescriptorSet __allocate(
			Device &device,
			VkDescriptorSetAllocateInfo const &allocInfo);
	};
}