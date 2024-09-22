#pragma once

#include "Executor.h"
#include "../Render/Engine.h"

namespace Frx
{
	template <std::derived_from<Render::Material> $Material>
	class Style : public Infra::Unique
	{
	public:
		template <typename ...$Args>
		Style(
			Executor &rcmdExecutor,
			Infra::ObjectHolder<Render::Engine> const &pEngine,
			$Args &&...args);

		virtual ~Style() noexcept override;

		[[nodiscard]]
		constexpr Infra::PointerHolder<$Material> const &_getMaterial() const noexcept;

	private:
		Executor &__rcmdExecutor;
		Infra::PointerHolder<$Material> __pMaterial;
	};

	template <std::derived_from<Render::Material> $Material>
	template <typename ...$Args>
	Style<$Material>::Style(
		Executor &rcmdExecutor,
		Infra::ObjectHolder<Render::Engine> const &pEngine,
		$Args &&...args) :
		__rcmdExecutor	{ rcmdExecutor }
	{
		__rcmdExecutor.silentRun([this, &pEngine]
		{
			__pMaterial.emplace(
				pEngine->createMaterial<$Material>(std::forward<$Args>(args)...));
		});
	}

	template <std::derived_from<Render::Material> $Material>
	Style<$Material>::~Style() noexcept
	{
		__rcmdExecutor.silentDestroy(std::move(__pMaterial));
	}

	template <std::derived_from<Render::Material> $Material>
	constexpr Infra::PointerHolder<$Material> const &Style<$Material>::_getMaterial() const noexcept
	{
		return __pMaterial;
	}
}