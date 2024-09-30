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
	class GlobalDescriptorManager : public Infra::Stateful<GlobalDescriptorManager>
	{
	public:
		struct BindingInfo
		{
		public:
			std::unordered_map<std::type_index, uint32_t> materialBufferLocations;
		};

		GlobalDescriptorManager(
			VK::PhysicalDevice const &physicalDevice,
			VK::Device &device,
			Infra::DeferredDeleter &deferredDeleter,
			ImageReferenceManager &imageReferenceManager,
			Dev::DescriptorUpdater &descriptorUpdater,
			ResourcePool &resourcePool,
			BindingInfo const &bindingInfo);

		virtual ~GlobalDescriptorManager() noexcept override;

		void setGlobalData(
			void const *pData,
			size_t size);

		void addMaterial(
			Material const *pMaterial);

		void removeMaterial(
			Material const *pMaterial);

		[[nodiscard]]
		uint32_t getMaterialIdOf(
			Material const *pMaterial) const;

		[[nodiscard]]
		constexpr VK::DescriptorSetLayout const &getGlobalDescSetLayout() const noexcept;

		[[nodiscard]]
		constexpr VK::DescriptorSetLayout const &getMaterialsDescSetLayout() const noexcept;

		[[nodiscard]]
		constexpr VK::DescriptorSetLayout const &getSampledImagesDescSetLayout() const noexcept;

		[[nodiscard]]
		constexpr VkDescriptorSet getGlobalDescSet() const noexcept;

		[[nodiscard]]
		constexpr VkDescriptorSet getMaterialsDescSet() const noexcept;

		[[nodiscard]]
		constexpr VkDescriptorSet getSampledImagesDescSet() const noexcept;

		[[nodiscard]]
		constexpr Infra::EventView<GlobalDescriptorManager const *> &
			getGlobalDataUpdateEvent() const noexcept;

	protected:
		virtual void _onValidate() override;

	private:
		VK::PhysicalDevice const &__physicalDevice;
		VK::Device &__device;
		Infra::DeferredDeleter &__deferredDeleter;
		ImageReferenceManager &__imageReferenceManager;
		Dev::DescriptorUpdater &__descUpdater;
		ResourcePool &__resourcePool;
		BindingInfo const __bindingInfo;

		std::unique_ptr<VK::DescriptorSetLayout> __pGlobalDescSetLayout;
		std::unique_ptr<VK::DescriptorSetLayout> __pMaterialsDescSetLayout;
		std::unique_ptr<VK::DescriptorSetLayout> __pSampledImagesDescSetLayout;

		std::shared_ptr<VK::DescriptorPool> __pGlobalDescPool;
		std::shared_ptr<VK::DescriptorPool> __pMaterialsDescPool;
		std::shared_ptr<VK::DescriptorPool> __pSampledImagesDescPool;

		std::array<VkDescriptorSet, Constants::DEFERRED_DELETER_QUEUE_SIZE> __globalDescSets;
		size_t __globalDescSetCursor{ };

		std::array<VkDescriptorSet, Constants::DEFERRED_DELETER_QUEUE_SIZE> __materialsDescSets;
		size_t __materialsDescSetCursor{ };

		std::array<VkDescriptorSet, Constants::DEFERRED_DELETER_QUEUE_SIZE> __sampledImagesDescSets;
		size_t __sampledImagesDescSetCursor{ };

		std::shared_ptr<Dev::MemoryBuffer> __pGlobalDataBuffer;
		std::unordered_map<std::type_index, std::unique_ptr<MaterialBufferBuilder>> __materialBufferBuilders;
		std::unordered_set<MaterialBufferBuilder *> __invalidatedMaterialBufferBuilders;

		bool __globalDataBufferUpdated{ };
		bool __materialsDescInvalidated{ };
		bool __sampledImagesDescInvalidated{ };

		uint32_t __sampledImagesDescArrSize{ 16U };

		Infra::EventListenerPtr<MaterialBufferBuilder *> __pMaterialBufferBuilderInvalidateListener;
		Infra::EventListenerPtr<ImageReferenceManager const *> __pSampledImagesDescInfosUpdateListener;

		mutable Infra::Event<GlobalDescriptorManager const *> __globalDataUpdateEvent;

		void __createGlobalDescSetLayout();
		void __createMaterialsDescSetLayout();
		void __createSampledImagesDescSetLayout();

		void __createGlobalDescPool();
		void __createMaterialsDescPool();
		void __createSampledImagesDescPool();

		void __allocateGlobalDescSets();
		void __allocateMaterialsDescSets();
		void __allocateSampledImagesDescSets();

		void __createMaterialBufferBuilders();

		void __validateGlobalDescSet();
		void __validateMaterialsDescSet();
		void __validateSampledImagesDescSet();

		constexpr void __advanceGlobalDescSet() noexcept;
		constexpr void __advanceMaterialsDescSet() noexcept;
		constexpr void __advanceSampledImagesDescSet() noexcept;
		void __growSampledImagesDescCount();

		void __onMaterialBufferBuilderInvalidated(
			MaterialBufferBuilder *pBuilder) noexcept;

		void __onSampledImagesDescInfosUpdated() noexcept;

		[[nodiscard]]
		MaterialBufferBuilder &__getMaterialBufferBuilderOf(
			std::type_index const &materialType);

		[[nodiscard]]
		MaterialBufferBuilder const &__getMaterialBufferBuilderOf(
			std::type_index const &materialType) const;
	};

	constexpr VK::DescriptorSetLayout const &GlobalDescriptorManager::getGlobalDescSetLayout() const noexcept
	{
		return *__pGlobalDescSetLayout;
	}

	constexpr VK::DescriptorSetLayout const &GlobalDescriptorManager::getMaterialsDescSetLayout() const noexcept
	{
		return *__pMaterialsDescSetLayout;
	}

	constexpr VK::DescriptorSetLayout const &GlobalDescriptorManager::getSampledImagesDescSetLayout() const noexcept
	{
		return *__pSampledImagesDescSetLayout;
	}

	constexpr VkDescriptorSet GlobalDescriptorManager::getGlobalDescSet() const noexcept
	{
		return __globalDescSets[__globalDescSetCursor];
	}

	constexpr VkDescriptorSet GlobalDescriptorManager::getMaterialsDescSet() const noexcept
	{
		return __materialsDescSets[__materialsDescSetCursor];
	}

	constexpr VkDescriptorSet GlobalDescriptorManager::getSampledImagesDescSet() const noexcept
	{
		return __sampledImagesDescSets[__sampledImagesDescSetCursor];
	}

	constexpr Infra::EventView<GlobalDescriptorManager const *> &
		GlobalDescriptorManager::getGlobalDataUpdateEvent() const noexcept
	{
		return __globalDataUpdateEvent;
	}

	constexpr void GlobalDescriptorManager::__advanceGlobalDescSet() noexcept
	{
		__globalDescSetCursor = ((__globalDescSetCursor + 1ULL) % __globalDescSets.size());
	}

	constexpr void GlobalDescriptorManager::__advanceMaterialsDescSet() noexcept
	{
		__materialsDescSetCursor = ((__materialsDescSetCursor + 1ULL) % __materialsDescSets.size());
	}

	constexpr void GlobalDescriptorManager::__advanceSampledImagesDescSet() noexcept
	{
		__sampledImagesDescSetCursor = ((__sampledImagesDescSetCursor + 1ULL) % __sampledImagesDescSets.size());
	}
}