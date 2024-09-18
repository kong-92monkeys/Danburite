#pragma once

#include "../Infra/DeferredRecycler.h"
#include "../Vulkan/DescriptorSetLayout.h"
#include "../Vulkan/DescriptorPool.h"
#include "../Device/DescriptorUpdater.h"
#include "Constants.h"
#include "ResourcePool.h"
#include "MaterialBufferBuilder.h"
#include <array>
#include <typeindex>

namespace Render
{
	class GlobalDescriptorManager : public Infra::Unique, public Infra::Stateful<GlobalDescriptorManager>
	{
	public:
		struct BindingInfo
		{
		public:
			std::unordered_map<std::type_index, uint32_t> materialBufferBindings;
			uint32_t sampledImageBinding{ };
		};

		GlobalDescriptorManager(
			VK::PhysicalDevice &physicalDevice,
			VK::Device &device,
			Infra::DeferredDeleter &deferredDeleter,
			Dev::DescriptorUpdater &descriptorUpdater,
			ResourcePool &resourcePool,
			BindingInfo const &bindingInfo);

		virtual ~GlobalDescriptorManager() noexcept override;

		void addMaterial(
			Material const *pMaterial);

		void removeMaterial(
			Material const *pMaterial);

		[[nodiscard]]
		uint32_t getMaterialIdOf(
			Material const *pMaterial) const noexcept;

		[[nodiscard]]
		constexpr VK::DescriptorSetLayout const &getDescSetLayout() const noexcept;

		[[nodiscard]]
		constexpr VkDescriptorSet getDescSet() const noexcept;

	protected:
		virtual void _onValidate() override;

	private:
		VK::PhysicalDevice &__physicalDevice;
		VK::Device &__device;
		Infra::DeferredDeleter &__deferredDeleter;
		Dev::DescriptorUpdater &__descriptorUpdater;
		ResourcePool &__resourcePool;
		BindingInfo const __bindingInfo;

		std::unique_ptr<VK::DescriptorSetLayout> __pDescSetLayout;
		std::shared_ptr<VK::DescriptorPool> __pDescPool;

		std::array<VkDescriptorSet, Constants::DEFERRED_DELETER_QUEUE_SIZE> __descSets;
		size_t __descSetCursor{ };

		std::unordered_map<std::type_index, std::unique_ptr<MaterialBufferBuilder>> __materialBufferBuilders;

		uint32_t __sampledImageDescCount{ 16U };

		Infra::EventListenerPtr<MaterialBufferBuilder *> __pMaterialBufferBuilderInvalidateListener;

		void __createDescSetLayout();
		void __createDescPool();
		void __allocateDescSets();
		void __createMaterialBufferBuilders();

		void __validateMaterialBufferBuilders();
		void __validateDescSet();

		constexpr void __advanceDescSet() noexcept;
		void __growSampledImageDescCount();
	};

	constexpr VK::DescriptorSetLayout const &GlobalDescriptorManager::getDescSetLayout() const noexcept
	{
		return *__pDescSetLayout;
	}

	constexpr VkDescriptorSet GlobalDescriptorManager::getDescSet() const noexcept
	{
		return __descSets[__descSetCursor];
	}

	constexpr void GlobalDescriptorManager::__advanceDescSet() noexcept
	{
		__descSetCursor = ((__descSetCursor + 1ULL) % __descSets.size());
	}
}