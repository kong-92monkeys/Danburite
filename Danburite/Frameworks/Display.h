#pragma once

#include "../Infra/Placeholder.h"
#include "../Render/Engine.h"

namespace Frx
{
	class Display : public Infra::Unique
	{
	public:
		Display(
			Infra::ThreadPool &rcmdExecutor,
			Infra::Placeholder<Render::Engine> const &pEngine,
			HINSTANCE hinstance,
			HWND hwnd);

		virtual ~Display() noexcept override;

	private:
		Infra::ThreadPool &__rcmdExecutor;
		Infra::Placeholder<Render::RenderTarget> __pRenderTarget;
	};
}
