#pragma once

#include "../Infra/Handle.h"
#include "Vulkan.h"

namespace VK
{
	class DescriptorSet : public Infra::Handle<VkDescriptorSet>
	{
	public:
		DescriptorSet(
			VkDescriptorSet handle) noexcept;

		virtual ~DescriptorSet() noexcept override = default;
	};
}