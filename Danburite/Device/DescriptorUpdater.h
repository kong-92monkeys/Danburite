#pragma once

#include "../Vulkan/Device.h"
#include <memory>

namespace Dev
{
	class DescriptorUpdater : public Infra::Unique
	{
	public:
		DescriptorUpdater(
			VK::Device &device) noexcept;

		void addBufferInfos(
			VkDescriptorSet hDescSet,
			uint32_t dstBinding,
			uint32_t dstArrayElement,
			uint32_t descriptorCount,
			VkDescriptorType descriptorType,
			VkDescriptorBufferInfo const *pInfos) noexcept;

		void update();

	private:
		VK::Device &__device;

		std::vector<VkWriteDescriptorSet> __writes;

		size_t __bufferInfoCount{ };
		std::vector<std::vector<VkDescriptorBufferInfo>> __bufferInfos;

		[[nodiscard]]
		std::vector<VkDescriptorBufferInfo> &__getBufferInfos() noexcept;
	};
}
