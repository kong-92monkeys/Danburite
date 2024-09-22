#include "Geometry.h"

namespace Frx
{
	Geometry::Geometry(
		Executor &rcmdExecutor,
		Infra::ObjectHolder<Render::Engine> const &engine) :
		__rcmdExecutor	{ rcmdExecutor }
	{
		__rcmdExecutor.silentRun([this, &engine]
		{
			__pMesh.emplace(engine->createMesh());
		});
	}

	Geometry::~Geometry() noexcept
	{
		__rcmdExecutor.silentDestroy(std::move(__pMesh));
	}

	void Geometry::createVertexBuffer(
		uint32_t bindingIndex,
		void const *pData,
		size_t size)
	{

	}

	void Geometry::updateVertexBuffer(
		uint32_t bindingIndex,
		void const *pData,
		size_t size,
		size_t offset)
	{

	}

	void Geometry::clearVertexBuffer(
		uint32_t bindingIndex)
	{

	}

	void Geometry::clearVertexBuffers()
	{

	}

	void Geometry::createIndexBuffer(
		VkIndexType type,
		void const *pData,
		size_t size)
	{

	}

	void Geometry::updateIndexBuffer(
		void const *pData,
		size_t size,
		size_t offset)
	{

	}

	void Geometry::clearIndexBuffer()
	{

	}
}