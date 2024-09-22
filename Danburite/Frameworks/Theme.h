#pragma once

#include "Executor.h"
#include "../Render/Engine.h"

namespace Frx
{
	template <std::derived_from<Render::Renderer> $Renderer>
	class Theme : public Infra::Unique
	{
	public:
		template <typename ...$Args>
		Theme(
			Executor &rcmdExecutor,
			Infra::ObjectHolder<Render::Engine> const &pEngine,
			$Args &&...args);

		virtual ~Theme() noexcept override;

		[[nodiscard]]
		constexpr Infra::PointerHolder<$Renderer> const &_getRenderer() const noexcept;

	private:
		Executor &__rcmdExecutor;
		Infra::PointerHolder<$Renderer> __pRenderer;
	};

	template <std::derived_from<Render::Renderer> $Renderer>
	template <typename ...$Args>
	Theme<$Renderer>::Theme(
		Executor &rcmdExecutor,
		Infra::ObjectHolder<Render::Engine> const &pEngine,
		$Args &&...args) :
		__rcmdExecutor	{ rcmdExecutor }
	{
		__rcmdExecutor.silentRun([this, &pEngine]
		{
			__pRenderer.emplace(
				pEngine->createRenderer<$Renderer>(std::forward<$Args>(args)...));
		});
	}

	template <std::derived_from<Render::Renderer> $Renderer>
	Theme<$Renderer>::~Theme() noexcept
	{
		__rcmdExecutor.silentDestroy(std::move(__pRenderer));
	}

	template <std::derived_from<Render::Renderer> $Renderer>
	constexpr Infra::PointerHolder<$Renderer> const &Theme<$Renderer>::_getRenderer() const noexcept
	{
		return __pRenderer;
	}
}