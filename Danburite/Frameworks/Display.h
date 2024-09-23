#pragma once

#include "Executor.h"
#include "../Render/Engine.h"

namespace Frx
{
	class Display : public Infra::Unique
	{
	public:
		Display(
			Executor &rcmdExecutor,
			Infra::ObjectHolder<Render::Engine> const &pEngine,
			HINSTANCE hinstance,
			HWND hwnd);

		virtual ~Display() noexcept override;

		[[nodiscard]]
		constexpr Infra::PointerHolder<Render::RenderTarget> const &_getRenderTarget() const noexcept;

	private:
		Executor &__rcmdExecutor;
		Infra::PointerHolder<Render::RenderTarget> __pRenderTarget;
	};

	constexpr Infra::PointerHolder<Render::RenderTarget> const &Display::_getRenderTarget() const noexcept
	{
		return __pRenderTarget;
	}
}