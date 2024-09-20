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
			std::unordered_map<std::type_index, uint32_t> materialBufferLocations;
		};

		GlobalDescriptorManager(
			VK::PhysicalDevice &physicalDevice,
			VK::Device &device,
			Infra::DeferredDeleter &deferredDeleter,
			ImageReferenceManager &imageReferenceManager,
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
		constexpr VK::DescriptorSetLayout const &getMaterialsDescSetLayout() const noexcept;

		[[nodiscard]]
		constexpr VK::DescriptorSetLayout const &getSampledImagesDescSetLayout() const noexcept;

		[[nodiscard]]
		constexpr VkDescriptorSet getMaterialsDescSet() const noexcept;

		[[nodiscard]]
		constexpr VkDescriptorSet getSampledImagesDescSet() const noexcept;

	protected:
		virtual void _onValidate() override;

	private:
		VK::PhysicalDevice &__physicalDevice;
		VK::Device &__device;
		Infra::DeferredDeleter &__deferredDeleter;
		ImageReferenceManager &__imageReferenceManager;
		Dev::DescriptorUpdater &__descriptorUpdater;
		ResourcePool &__resourcePool;
		BindingInfo const __bindingInfo;

		std::unique_ptr<VK::DescriptorSetLayout> __pMaterialsDescSetLayout;
		std::unique_ptr<VK::DescriptorSetLayout> __pSampledImagesDescSetLayout;

		std::shared_ptr<VK::DescriptorPool> __pMaterialsDescPool;
		std::shared_ptr<VK::DescriptorPool> __pSampledImagesDescPool;

		std::array<VkDescriptorSet, Constants::DEFERRED_DELETER_QUEUE_SIZE> __materialsDescSets;
		size_t __materialDescSetCursor{ };

		std::array<VkDescriptorSet, Constants::DEFERRED_DELETER_QUEUE_SIZE> __sampledImagesDescSets;
		size_t __sampledImagesDescSetCursor{ };

		std::unordered_map<std::type_index, std::unique_ptr<MaterialBufferBuilder>> __materialBufferBuilders;

		std::unordered_set<MaterialBufferBuilder *> __invalidatedMaterialBufferBuilders;
		bool __materialsDescInvalidated{ };
		bool __sampledImagesDescInvalidated{ };

		uint32_t __sampledImagesDescArrSize{ 16U };

		Infra::EventListenerPtr<MaterialBufferBuilder *> __pMaterialBufferBuilderInvalidateListener;
		Infra::EventListenerPtr<ImageReferenceManager const *> __pSampledImagesDescInfosUpdateListener;

		void __createMaterialsDescSetLayout();
		void __createSampledImagesDescSetLayout();

		void __createMaterialsDescPool();
		void __createSampledImagesDescPool();

		void __allocateMaterialsDescSets();
		void __allocateSampledImagesDescSets();

		void __createMaterialBufferBuilders();

		void __validateMaterialsDescSet();
		void __validateSampledImagesDescSet();

		constexpr void __advanceMaterialsDescSet() noexcept;
		constexpr void __advanceSampledImagesDescSet() noexcept;
		void __growSampledImagesDescCount();

		void __onMaterialBufferBuilderInvalidated(
			MaterialBufferBuilder *pBuilder) noexcept;

		void __onSampledImagesDescInfosUpdated() noexcept;
	};

	constexpr VK::DescriptorSetLayout const &GlobalDescriptorManager::getMaterialsDescSetLayout() const noexcept
	{
		return *__pMaterialsDescSetLayout;
	}

	constexpr VK::DescriptorSetLayout const &GlobalDescriptorManager::getSampledImagesDescSetLayout() const noexcept
	{
		return *__pSampledImagesDescSetLayout;
	}

	constexpr VkDescriptorSet GlobalDescriptorManager::getMaterialsDescSet() const noexcept
	{
		return __materialsDescSets[__materialDescSetCursor];
	}

	constexpr VkDescriptorSet GlobalDescriptorManager::getSampledImagesDescSet() const noexcept
	{
		return __sampledImagesDescSets[__sampledImagesDescSetCursor];
	}

	constexpr void GlobalDescriptorManager::__advanceMaterialsDescSet() noexcept
	{
		__materialDescSetCursor = ((__materialDescSetCursor + 1ULL) % __materialsDescSets.size());
	}

	constexpr void GlobalDescriptorManager::__advanceSampledImagesDescSet() noexcept
	{
		__sampledImagesDescSetCursor = ((__sampledImagesDescSetCursor + 1ULL) % __sampledImagesDescSets.size());
	}
}