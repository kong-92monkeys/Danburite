#include "RenderObject.h"

namespace Render
{
	RenderObject::RenderObject()
	{
		__pMeshUpdateListener =
			Infra::EventListener<Mesh const *>::bind(
				&RenderObject::__onMeshUpdated, this);

		__pMeshVertexAttribFlagsChangeListener =
			Infra::EventListener<Mesh const *, uint32_t, uint32_t>::bind(
				&RenderObject::__onMeshVertexAttribFlagsChanged, this);

		__pMaterialPackMaterialChangeListener =
			Infra::EventListener<MaterialPack const *, std::type_index, Material const *, Material const *>::bind(
				&RenderObject::__onMaterialPackMaterialChanged, this,
				std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

		__pMaterialUpdateListener =
			Infra::EventListener<MaterialPack const *>::bind(
				&RenderObject::__onMaterialUpdated, this);

		__pMaterialValidChangeListener =
			Infra::EventListener<MaterialPack const *>::bind(
				&RenderObject::__onMaterialValidChanged, this);

		auto pMaterialPack{ std::make_unique<MaterialPack>() };
		pMaterialPack->getMaterialChangeEvent() += __pMaterialPackMaterialChangeListener;
		pMaterialPack->getMaterialUpdateEvent() += __pMaterialUpdateListener;
		pMaterialPack->getMaterialValidChangeEvent() += __pMaterialValidChangeListener;

		__materialPack2Index[pMaterialPack.get()] = 0U;
		__materialPacks.emplace_back(std::move(pMaterialPack));
	}

	void RenderObject::setRenderer(
		Renderer const *const pRenderer)
	{
		if (__pRenderer == pRenderer)
			return;

		auto const pPrevRenderer{ __pRenderer };
		__pRenderer = pRenderer;

		__rendererChangeEvent.invoke(this, pPrevRenderer, __pRenderer);
		__validateDrawable();

		__needRedrawEvent.invoke(this);
	}

	void RenderObject::setMesh(
		Mesh const *const pMesh)
	{
		if (__pMesh == pMesh)
			return;

		auto const pPrevMesh{ __pMesh };
		if (pPrevMesh)
		{
			pPrevMesh->getUpdateEvent() -= __pMeshUpdateListener;
			pPrevMesh->getVertexAttribFlagsChangeEvent() -= __pMeshVertexAttribFlagsChangeListener;
		}

		__pMesh = pMesh;
		if (__pMesh)
		{
			__pMesh->getUpdateEvent() += __pMeshUpdateListener;
			__pMesh->getVertexAttribFlagsChangeEvent() += __pMeshVertexAttribFlagsChangeListener;
		}

		__meshChangeEvent.invoke(this, pPrevMesh, __pMesh);
		__validateDrawable();

		__needRedrawEvent.invoke(this);
	}

	void RenderObject::setDrawParam(
		DrawParam const *const pDrawParam)
	{
		if (__pDrawParam == pDrawParam)
			return;

		auto const pPrevDrawParam{ __pDrawParam };
		__pDrawParam = pDrawParam;

		__drawParamChangeEvent.invoke(this, pPrevDrawParam, __pDrawParam);
		__validateDrawable();

		__needRedrawEvent.invoke(this);
	}

	void RenderObject::setInstanceCount(
		uint32_t const count)
	{
		uint32_t const prevCount{ getInstanceCount() };
		if (prevCount == count)
			return;

		std::vector<std::pair<uint32_t, std::shared_ptr<MaterialPack const>>> expiredPacks;

		for (uint32_t expiredIt{ prevCount - 1U }; expiredIt >= count; --expiredIt)
		{
			auto &pExpiredPack{ __materialPacks[expiredIt] };
			pExpiredPack->getMaterialChangeEvent() -= __pMaterialPackMaterialChangeListener;

			__materialPack2Index.erase(pExpiredPack.get());
			expiredPacks.emplace_back(expiredIt, std::move(pExpiredPack));
		}

		for (uint32_t instanceIt{ prevCount }; instanceIt < count; ++instanceIt)
		{
			auto pMaterialPack{ std::make_unique<MaterialPack>() };
			pMaterialPack->getMaterialChangeEvent() += __pMaterialPackMaterialChangeListener;

			__materialPack2Index[pMaterialPack.get()] = instanceIt;
			__materialPacks.emplace_back(std::move(pMaterialPack));
		}

		for (auto const &[instanceIndex, pExpiredPack] : expiredPacks)
		{
			for (const auto pMaterial : *pExpiredPack)
				__materialChangeEvent.invoke(this, instanceIndex, typeid(*pMaterial), pMaterial, nullptr);
		}

		__instanceCountChangeEvent.invoke(this, prevCount, count);

		if (prevCount < count)
			__validateDrawable();

		__needRedrawEvent.invoke(this);
	}

	void RenderObject::setVisible(
		bool const visible)
	{
		if (__visible == visible)
			return;

		__visible = visible;
		__validateDrawable();

		__needRedrawEvent.invoke(this);
	}

	void RenderObject::draw(
		VK::CommandBuffer &cmdBuffer,
		uint32_t baseId) const noexcept
	{
		__pDrawParam->draw(cmdBuffer, getInstanceCount(), baseId);
	}

	bool RenderObject::__resolveDrawable() const noexcept
	{
		if (!__pRenderer || !__pMesh || !__pDrawParam || !__visible)
			return false;

		if (!(__pRenderer->isValidVertexAttribFlags(__pMesh->getVertexAttribFlags())))
			return false;

		for (auto const &pMaterialPack : __materialPacks)
		{
			if (!(__pRenderer->isValidMaterialPack(*pMaterialPack)))
				return false;
		}

		return true;
	}

	void RenderObject::__validateDrawable()
	{
		bool const curDrawable{ __resolveDrawable() };
		if (__drawable == curDrawable)
			return;

		__drawable = curDrawable;
		__drawableChangeEvent.invoke(this, curDrawable);
	}

	void RenderObject::__onMeshUpdated()
	{
		__needRedrawEvent.invoke(this);
	}

	void RenderObject::__onMeshVertexAttribFlagsChanged()
	{
		__validateDrawable();
		__needRedrawEvent.invoke(this);
	}

	void RenderObject::__onMaterialPackMaterialChanged(
		MaterialPack const *pPack,
		std::type_index const &type,
		Material const *pPrev,
		Material const *pCur)
	{
		uint32_t const instanceIndex{ __materialPack2Index.at(pPack) };
		__materialChangeEvent.invoke(this, instanceIndex, type, pPrev, pCur);
		__validateDrawable();

		__needRedrawEvent.invoke(this);
	}

	void RenderObject::__onMaterialUpdated()
	{
		__needRedrawEvent.invoke(this);
	}

	void RenderObject::__onMaterialValidChanged()
	{
		__validateDrawable();
	}
}