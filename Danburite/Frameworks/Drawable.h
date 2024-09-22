#pragma once

#include "Executor.h"
#include "../Render/Engine.h"

namespace Frx
{
	class Drawable : public Infra::Unique
	{
	public:
		Drawable(
			Executor &rcmdExecutor);

		virtual ~Drawable() noexcept override;

		[[nodiscard]]
		constexpr Infra::ObjectHolder<Render::RenderObject> const &_getRenderObject() const noexcept;

	private:
		Executor &__rcmdExecutor;
		Infra::ObjectHolder<Render::RenderObject> __pRenderObject;
	};

	constexpr Infra::ObjectHolder<Render::RenderObject> const &Drawable::_getRenderObject() const noexcept
	{
		return __pRenderObject;
	}
}