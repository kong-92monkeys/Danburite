#pragma once

#include "Renderer.h"
#include "Mesh.h"
#include "DrawParam.h"

namespace Render
{
	class RenderObject : public Infra::Unique
	{
	public:
		RenderObject();

		[[nodiscard]]
		constexpr std::shared_ptr<Renderer const> const &getRenderer() const noexcept;
		void setRenderer(
			std::shared_ptr<Renderer const> const &pRenderer);

		[[nodiscard]]
		constexpr Mesh const *getMesh() const noexcept;
		void setMesh(
			Mesh const *pMesh);

		[[nodiscard]]
		constexpr std::shared_ptr<DrawParam const> const &getDrawParam() const noexcept;
		void setDrawParam(std::shared_ptr<DrawParam const> const &pDrawParam);

		[[nodiscard]]
		constexpr MaterialPack &getMaterialPackOf(
			uint32_t instanceIndex) noexcept;

		[[nodiscard]]
		constexpr MaterialPack const &getMaterialPackOf(
			uint32_t instanceIndex) const noexcept;

		[[nodiscard]]
		constexpr uint32_t getInstanceCount() const noexcept;
		void setInstanceCount(const uint32_t count);

		[[nodiscard]]
		constexpr bool isVisible() const noexcept;
		void setVisible(const bool visible);

		[[nodiscard]]
		constexpr bool isDrawable() const noexcept;

		void draw(
			VK::CommandBuffer &cmdBuffer,
			uint32_t baseId) const noexcept;

		[[nodiscard]]
		constexpr Infra::EventView<RenderObject const *, Renderer const *, Renderer const *> &
			getRendererChangeEvent() const noexcept;

		[[nodiscard]]
		constexpr Infra::EventView<RenderObject const *, Mesh const *, Mesh const *> &
			getMeshChangeEvent() const noexcept;

		[[nodiscard]]
		constexpr Infra::EventView<RenderObject const *, DrawParam const *, DrawParam const *> &
			getDrawParamChangeEvent() const noexcept;

		[[nodiscard]]
		constexpr Infra::EventView<RenderObject const *, uint32_t, std::type_index, Material const *, Material const *> &
			getMaterialChangeEvent() const noexcept;

		[[nodiscard]]
		constexpr Infra::EventView<RenderObject const *, uint32_t, uint32_t> &
			getInstanceCountChangeEvent() const noexcept;

		[[nodiscard]]
		constexpr Infra::EventView<RenderObject const *, bool> &
			getDrawableChangeEvent() const noexcept;

	private:
		std::shared_ptr<Renderer const> __pRenderer;
		Mesh const *__pMesh{ };
		std::shared_ptr<DrawParam const> __pDrawParam;
		std::vector<std::unique_ptr<MaterialPack>> __materialPacks;

		bool __visible{ true };
		bool __drawable{ };

		std::unordered_map<MaterialPack const *, uint32_t> __materialPack2Index;

		mutable Infra::Event<RenderObject const *, Renderer const *, Renderer const *>
			__rendererChangeEvent;

		mutable Infra::Event<RenderObject const *, Mesh const *, Mesh const *>
			__meshChangeEvent;

		mutable Infra::Event<RenderObject const *, DrawParam const *, DrawParam const *>
			__drawParamChangeEvent;

		mutable Infra::Event<RenderObject const *, uint32_t, std::type_index, Material const *, Material const *>
			__materialChangeEvent;

		mutable Infra::Event<RenderObject const *, uint32_t, uint32_t>
			__instanceCountChangeEvent;

		mutable Infra::Event<RenderObject const *, bool>
			__drawableChangeEvent;

		Infra::EventListenerPtr<MaterialPack const *, std::type_index, Material const *, Material const *>
			__pMaterialPackMaterialChangeListener;

		Infra::EventListenerPtr<MaterialPack const *>
			__pMaterialPackMaterialValidChangeListener;

		[[nodiscard]]
		bool __resolveDrawable() const noexcept;
		void __validateDrawable();

		void __onMaterialPackMaterialChanged(
			MaterialPack const *pPack,
			std::type_index const &type,
			Material const *pPrev,
			Material const *pCur);

		void __onMaterialPackMaterialValidChanged();
	};

	constexpr std::shared_ptr<Renderer const> const &RenderObject::getRenderer() const noexcept
	{
		return __pRenderer;
	}

	constexpr Mesh const *RenderObject::getMesh() const noexcept
	{
		return __pMesh;
	}

	constexpr std::shared_ptr<DrawParam const> const &RenderObject::getDrawParam() const noexcept
	{
		return __pDrawParam;
	}

	constexpr uint32_t RenderObject::getInstanceCount() const noexcept
	{
		return static_cast<uint32_t>(__materialPacks.size());
	}

	constexpr MaterialPack &RenderObject::getMaterialPackOf(
		uint32_t const instanceIndex) noexcept
	{
		return *(__materialPacks[instanceIndex]);
	}

	constexpr MaterialPack const &RenderObject::getMaterialPackOf(
		uint32_t const instanceIndex) const noexcept
	{
		return *(__materialPacks[instanceIndex]);
	}

	constexpr bool RenderObject::isVisible() const noexcept
	{
		return __visible;
	}

	constexpr bool RenderObject::isDrawable() const noexcept
	{
		return __drawable;
	}

	constexpr Infra::EventView<RenderObject const *, Renderer const *, Renderer const *> &
		RenderObject::getRendererChangeEvent() const noexcept
	{
		return __rendererChangeEvent;
	}

	constexpr Infra::EventView<RenderObject const *, Mesh const *, Mesh const *> &
		RenderObject::getMeshChangeEvent() const noexcept
	{
		return __meshChangeEvent;
	}

	constexpr Infra::EventView<RenderObject const *, DrawParam const *, DrawParam const *> &
		RenderObject::getDrawParamChangeEvent() const noexcept
	{
		return __drawParamChangeEvent;
	}

	constexpr Infra::EventView<RenderObject const *, uint32_t, std::type_index, Material const *, Material const *> &
		RenderObject::getMaterialChangeEvent() const noexcept
	{
		return __materialChangeEvent;
	}

	constexpr Infra::EventView<RenderObject const *, uint32_t, uint32_t> &
		RenderObject::getInstanceCountChangeEvent() const noexcept
	{
		return __instanceCountChangeEvent;
	}

	constexpr Infra::EventView<RenderObject const *, bool> &
		RenderObject::getDrawableChangeEvent() const noexcept
	{
		return __drawableChangeEvent;
	}
}