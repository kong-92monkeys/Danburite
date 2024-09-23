#pragma once

#include "Executor.h"
#include "../Render/Engine.h"

namespace Frx
{
	class Canvas : public Infra::Unique
	{
	public:
		Canvas(
			Executor &rcmdExecutor,
			Infra::ObjectHolder<Render::Engine> const &pEngine);

		virtual ~Canvas() noexcept override;

		[[nodiscard]]
		constexpr Infra::PointerHolder<Render::Layer> const &_getLayer() const noexcept;

	private:
		Executor &__rcmdExecutor;
		Infra::PointerHolder<Render::Layer> __pLayer;
	};

	constexpr Infra::PointerHolder<Render::Layer> const &Canvas::_getLayer() const noexcept
	{
		return __pLayer;
	}
}