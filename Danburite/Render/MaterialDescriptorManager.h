#pragma once

#include "../Infra/Stateful.h"
#include "../Infra/IdAllocator.h"
#include "../Infra/GenericBuffer.h"
#include "../Vulkan/DescriptorSetLayout.h"
#include "../Vulkan/DescriptorPool.h"
#include "Material.h"
#include "ResourcePool.h"
#include "Constants.h"
#include <array>

namespace Render
{
	class MaterialDescriptorManager : public Infra::Unique, public Infra::Stateful<MaterialDescriptorManager>
	{
	public:
		MaterialDescriptorManager(
			VK::Device &device,
			ResourcePool &resourcePool,
			std::unordered_map<std::type_index, uint32_t> const &materialTypeIds);

		void registerMaterial(
			Material const *pMaterial) noexcept;

		void unregisterMaterial(
			Material const *pMaterial) noexcept;

		[[nodiscard]]
		uint32_t getIdOf(
			Material const *pMaterial) const noexcept;

		[[nodiscard]]
		constexpr VK::DescriptorSetLayout const &getDescriptorSetLayout() const noexcept;

		[[nodiscard]]
		VK::DescriptorSet const &getDescriptorSet() const noexcept;

	protected:
		virtual void _onValidate() override;

	private:
		VK::Device &__device;
		ResourcePool &__resourcePool;
		std::unordered_map<std::type_index, uint32_t> const __materialTypeIds;

		Infra::IdAllocator<uint32_t> __materialIdAllocator;
		std::unordered_map<Material const *, std::pair<size_t, uint32_t>> __materialRefIdMap;

		Infra::GenericBuffer __materialHostBuffer;
		std::shared_ptr<Dev::MemoryBuffer> __pMaterialBuffer;

		bool __materialBufferInvalidated{ };

		std::unique_ptr<VK::DescriptorSetLayout> __pDescSetLayout;
		std::unique_ptr<VK::DescriptorPool> __pDescPool;

		//std::array<std::unique_ptr<VK::DescriptorSet>, Constants::MAX_IN_FLIGHT_FRAME_COUNT> __descSets;
		uint32_t __descSetCursor{ };

		Infra::EventListenerPtr<Material const *> __pMaterialUpdateListener;

		void __createDescSetLayout();
		void __createDescPool();
		void __createDescSets();

		void __validateMaterialHostBuffer(
			Material const *pMaterial) noexcept;

		void __validateMaterialBuffer();

		void __onMaterialUpdated(
			Material const *pMaterial) noexcept;
	};

	constexpr VK::DescriptorSetLayout const &MaterialDescriptorManager::getDescriptorSetLayout() const noexcept
	{
		return *__pDescSetLayout;
	}
}