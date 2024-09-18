#pragma once

#include "../Infra/IdAllocator.h"
#include "../Infra/Stateful.h"
#include "../Infra/GenericBuffer.h"
#include "Material.h"
#include "ResourcePool.h"

namespace Render
{
	class MaterialBufferBuilder : public Infra::Unique, public Infra::Stateful<MaterialBufferBuilder>
	{
	public:
		MaterialBufferBuilder(
			ResourcePool &resourcePool) noexcept;

		virtual ~MaterialBufferBuilder() noexcept override;

		void addMaterial(
			Material const *pMaterial);

		void removeMaterial(
			Material const *pMaterial);

		[[nodiscard]]
		uint32_t getIdOf(
			Material const *pMaterial) const noexcept;

		[[nodiscard]]
		Dev::MemoryBuffer const &getMaterialBuffer() const noexcept;

	protected:
		virtual void _onValidate() override;

	private:
		ResourcePool &__resourcePool;

		Infra::IdAllocator<uint32_t> __materialIdAllocator;
		std::unordered_map<const Material *, std::pair<size_t, uint32_t>> __materialRefIdMap;

		Infra::GenericBuffer __materialHostBuffer;
		std::shared_ptr<Dev::MemoryBuffer> __pMaterialBuffer;
		bool __materialBufferInvalidated{ };

		Infra::EventListenerPtr<Material const *> __pMaterialUpdateListener;

		void __registerMaterial(
			Material const *pMaterial);

		void __unregisterMaterial(
			Material const *pMaterial);

		void __validateMaterialHostBuffer(
			Material const *const pMaterial) noexcept;

		void __validateMaterialBuffer();

		void __onMaterialUpdated(
			Material const *pMaterial) noexcept;
	};
}
