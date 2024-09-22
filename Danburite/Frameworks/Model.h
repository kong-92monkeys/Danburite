#pragma once

#include "Executor.h"
#include "../Render/Engine.h"

namespace Frx
{
	class Model : public Infra::Unique
	{
	public:
		Model(
			Executor &rcmdExecutor,
			Infra::ObjectHolder<Render::Engine> const &pEngine);

		virtual ~Model() noexcept override;

		void createVertexBuffer(
			uint32_t bindingIndex,
			void const *pData,
			size_t size);

		void updateVertexBuffer(
			uint32_t bindingIndex,
			void const *pData,
			size_t size,
			size_t offset);

		void clearVertexBuffer(
			uint32_t bindingIndex);

		void clearVertexBuffers();

		void createIndexBuffer(
			VkIndexType type,
			void const *pData,
			size_t size);

		void updateIndexBuffer(
			void const *pData,
			size_t size,
			size_t offset);

		void clearIndexBuffer();

		[[nodiscard]]
		constexpr Infra::PointerHolder<Render::Mesh> const &_getMesh() const noexcept;

	private:
		Executor &__rcmdExecutor;
		Infra::PointerHolder<Render::Mesh> __pMesh;
	};

	constexpr Infra::PointerHolder<Render::Mesh> const &Model::_getMesh() const noexcept
	{
		return __pMesh;
	}
}