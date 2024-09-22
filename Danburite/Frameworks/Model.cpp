#include "Model.h"

namespace Frx
{
	Model::Model(
		Executor &rcmdExecutor,
		Infra::ObjectHolder<Render::Engine> const &pEngine) :
		__rcmdExecutor	{ rcmdExecutor }
	{
		__rcmdExecutor.silentRun([this, &pEngine]
		{
			__pMesh.emplace(pEngine->createMesh());
		});
	}

	Model::~Model() noexcept
	{
		__rcmdExecutor.silentDestroy(std::move(__pMesh));
	}

	void Model::createVertexBuffer(
		uint32_t const bindingIndex,
		void const *const pData,
		size_t const size)
	{
		__rcmdExecutor.silentRun([=, this]
		{
			__pMesh->createVertexBuffer(bindingIndex, pData, size);
		});
	}

	void Model::updateVertexBuffer(
		uint32_t const bindingIndex,
		void const *const pData,
		size_t const size,
		size_t const offset)
	{
		__rcmdExecutor.silentRun([=, this]
		{
			__pMesh->updateVertexBuffer(bindingIndex, pData, size, offset);
		});
	}

	void Model::clearVertexBuffer(
		uint32_t const bindingIndex)
	{
		__rcmdExecutor.silentRun([=, this]
		{
			__pMesh->clearVertexBuffer(bindingIndex);
		});
	}

	void Model::clearVertexBuffers()
	{
		__rcmdExecutor.silentRun([this]
		{
			__pMesh->clearVertexBuffers();
		});
	}

	void Model::createIndexBuffer(
		VkIndexType const type,
		void const *const pData,
		size_t const size)
	{
		__rcmdExecutor.silentRun([=, this]
		{
			__pMesh->createIndexBuffer(type, pData, size);
		});
	}

	void Model::updateIndexBuffer(
		void const *const pData,
		size_t const size,
		size_t const offset)
	{
		__rcmdExecutor.silentRun([=, this]
		{
			__pMesh->updateIndexBuffer(pData, size, offset);
		});
	}

	void Model::clearIndexBuffer()
	{
		__rcmdExecutor.silentRun([this]
		{
			__pMesh->clearIndexBuffer();
		});
	}
}