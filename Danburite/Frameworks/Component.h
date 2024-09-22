#pragma once

#include "../Infra/PointerHolder.h"
#include "../Render/RenderObject.h"
#include "Executor.h"

namespace Frx
{
	class Component : public Infra::Unique
	{
	public:
		void init(
			Executor &rcmdExecutor,
			Infra::PointerHolder<Render::RenderObject> const &pRenderObject);

	protected:
		virtual void _onInit();

		[[nodiscard]]
		constexpr Executor &_getRcmdExecutor() const noexcept;

		[[nodiscard]]
		constexpr Infra::PointerHolder<Render::RenderObject> const &
			_getRenderObject() const noexcept;

	private:
		Executor *__pRcmdExecutor{ };
		Infra::PointerHolder<Render::RenderObject> __pRenderObject;
	};

	constexpr Executor &Component::_getRcmdExecutor() const noexcept
	{
		return *__pRcmdExecutor;
	}

	constexpr Infra::PointerHolder<Render::RenderObject> const &
		Component::_getRenderObject() const noexcept
	{
		return __pRenderObject;
	}
}