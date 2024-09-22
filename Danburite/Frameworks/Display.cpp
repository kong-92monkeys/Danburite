#include "Display.h"

namespace Frx
{
	Display::Display(
		Executor &rcmdExecutor,
		Infra::ObjectHolder<Render::Engine> const &pEngine,
		HINSTANCE const hinstance,
		HWND const hwnd) :
		__rcmdExecutor	{ rcmdExecutor }
	{
		__rcmdExecutor.run([this, &pEngine, hinstance, hwnd]
		{
			__pRenderTarget.emplace(pEngine->createRenderTarget(hinstance, hwnd));
		}).wait();
	}

	Display::~Display() noexcept
	{
		__rcmdExecutor.destroy(std::move(__pRenderTarget)).wait();
	}
}