#pragma once

#include "Device.h"
#include <unordered_map>

namespace VK
{
	class DescriptorSetLayout : public Infra::Handle<VkDescriptorSetLayout>
	{
	public:
		DescriptorSetLayout(
			Device &device,
			VkDescriptorSetLayoutCreateInfo const &createInfo);

		virtual ~DescriptorSetLayout() noexcept override;

		[[nodiscard]]
		constexpr std::unordered_map<VkDescriptorType, uint32_t> const &getDescCountMap() const noexcept;

	private:
		Device &__device;

		std::unordered_map<VkDescriptorType, uint32_t> __descCountMap;

		[[nodiscard]]
		static VkDescriptorSetLayout __create(
			Device &device,
			VkDescriptorSetLayoutCreateInfo const &createInfo);
	};

	constexpr std::unordered_map<VkDescriptorType, uint32_t> const &DescriptorSetLayout::getDescCountMap() const noexcept
	{
		return __descCountMap;
	}
}