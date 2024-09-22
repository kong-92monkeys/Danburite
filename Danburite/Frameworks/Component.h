#pragma once

#include "../Infra/Placeholder.h"
#include "../Infra/ThreadPool.h"
#include "../Render/RenderObject.h"

namespace Frx
{
	class Component : public Infra::Unique
	{
	public:
		void init(
			Infra::ThreadPool &rcmdExecutor,
			Infra::Placeholder<std::shared_ptr<Render::RenderObject>> const &renderObject);

	protected:
		virtual void _onInit();

		[[nodiscard]]
		constexpr Infra::ThreadPool &_getRcmdExecutor() const noexcept;

		[[nodiscard]]
		constexpr Infra::Placeholder<std::shared_ptr<Render::RenderObject>> const &
			_getRenderObject() const noexcept;

	private:
		Infra::ThreadPool *__pRcmdExecutor{ };
		Infra::Placeholder<std::shared_ptr<Render::RenderObject>> const *__pRenderObject{ };
	};

	constexpr Infra::ThreadPool &Component::_getRcmdExecutor() const noexcept
	{
		return *__pRcmdExecutor;
	}

	constexpr Infra::Placeholder<std::shared_ptr<Render::RenderObject>> const &
		Component::_getRenderObject() const noexcept
	{
		return *__pRenderObject;
	}
}