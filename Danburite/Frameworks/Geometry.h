#pragma once

#include "Executor.h"
#include "../Render/Engine.h"

namespace Frx
{
	class Geometry : public Infra::Unique
	{
	public:
		Geometry(
			Executor &rcmdExecutor,
			Infra::ObjectHolder<Render::Engine> const &pEngine);

		virtual ~Geometry() noexcept override;

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

	private:
		Executor &__rcmdExecutor;
		Infra::PointerHolder<Render::Mesh> __pMesh;
	};
}
