#pragma once

#include "../Infra/DeferredRecycler.h"
#include "../Vulkan/DescriptorSetLayout.h"
#include "../Vulkan/DescriptorPool.h"
#include "ResourcePool.h"
#include <typeindex>

namespace Render
{
	class GlobalDescriptorManager : public Infra::Unique
	{
	public:
		GlobalDescriptorManager(
			VK::PhysicalDevice &physicalDevice,
			VK::Device &device,
			Infra::DeferredDeleter &deferredDeleter,
			ResourcePool &resourcePool,
			std::unordered_map<std::type_index, uint32_t> const &materialTypeIds);

		virtual ~GlobalDescriptorManager() noexcept override;

	private:
		VK::PhysicalDevice &__physicalDevice;
		VK::Device &__device;
		Infra::DeferredDeleter &__deferredDeleter;
		ResourcePool &__resourcePool;
		std::unordered_map<std::type_index, uint32_t> const __materialTypeIds;

		std::unique_ptr<VK::DescriptorSetLayout> __pDescSetLayout;
		std::unique_ptr<VK::DescriptorPool> __pDescPool;

		uint32_t __sampledImageDescCount{ 128U };

		void __createDescSetLayout();
		void __createDescPool();
	};
}