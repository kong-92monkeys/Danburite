#pragma once

#include "../Infra/DeferredRecycler.h"
#include "../Vulkan/DescriptorSetLayout.h"
#include "ResourcePool.h"
#include <typeindex>

namespace Render
{
	class GlobalDescriptorManager : public Infra::Unique
	{
	public:
		GlobalDescriptorManager(
			VK::Device &device,
			Infra::DeferredDeleter &deferredDeleter,
			ResourcePool &resourcePool,
			std::unordered_map<std::type_index, uint32_t> const &materialTypeIds);

		virtual ~GlobalDescriptorManager() noexcept override;

	private:
		VK::Device &__device;
		Infra::DeferredDeleter &__deferredDeleter;
		ResourcePool &__resourcePool;
		std::unordered_map<std::type_index, uint32_t> const __materialTypeIds;

		std::unique_ptr<VK::DescriptorSetLayout> __pDescSetLayout;

		void __createDescSetLayout();
	};
}