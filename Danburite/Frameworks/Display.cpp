#include "Display.h"

namespace Frx
{
	Display::Display(
		Infra::ThreadPool &rcmdExecutor,
		Infra::Placeholder<Render::Engine> const &engine,
		HINSTANCE const hinstance,
		HWND const hwnd) :
		__rcmdExecutor	{ rcmdExecutor }
	{
		__rcmdExecutor.run([this, &engine, hinstance, hwnd]
		{
			__renderTarget.instantiate(engine->createRenderTarget(hinstance, hwnd));
		}).wait();
	}

	Display::~Display() noexcept
	{
		__rcmdExecutor.run([this]
		{
			__renderTarget.reset();
		}).wait();
	}
}