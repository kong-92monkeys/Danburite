#include "Display.h"

namespace Frx
{
	Display::Display(
		Infra::Executor &rcmdExecutor,
		Render::Engine &renderEngine,
		HINSTANCE const hinstance,
		HWND const hwnd) :
		__rcmdExecutor	{ rcmdExecutor },
		__renderEngine	{ renderEngine }
	{
		__rcmdExecutor.run([this, hinstance, hwnd]
		{
			__pRenderTargetNeedRedrawListener =
				Infra::EventListener<Render::RenderTarget const *>::bind(
				&Display::__rcmd_onRenderTargetNeedRedraw, this);

			__pRenderTarget = __renderEngine.createRenderTarget(hinstance, hwnd);
			__pRenderTarget->getNeedRedrawEvent() += __pRenderTargetNeedRedrawListener;
		}).wait();
	}

	Display::~Display() noexcept
	{
		__rcmdExecutor.run([this]
		{
			delete __pRenderTarget;
			__pRenderTargetNeedRedrawListener = nullptr;
		}).wait();
	}

	void Display::sync()
	{
		__rcmdExecutor.run([this]
		{
			__pRenderTarget->sync();
		}).wait();
	}

	void Display::draw()
	{
		__rcmdExecutor.silentRun([this]
		{
			__renderEngine.reserveRender(__pRenderTarget);
		});
	}

	void Display::__rcmd_onRenderTargetNeedRedraw()
	{
		__rcmdExecutor.silentRun([this]
		{
			__renderEngine.reserveRender(__pRenderTarget);
		});
	}
}