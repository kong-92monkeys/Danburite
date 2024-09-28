#include "Display.h"

namespace Frx
{
	Display::Display(
		Infra::ThreadPool &rcmdExecutor,
		Render::Engine &renderEngine,
		HINSTANCE const hinstance,
		HWND const hwnd,
		bool const useDepthBuffer,
		bool const useStencilBuffer) :
		__rcmdExecutor	{ rcmdExecutor },
		__renderEngine	{ renderEngine }
	{
		__rcmdExecutor.run([this, hinstance, hwnd, useDepthBuffer, useStencilBuffer]
		{
			__pRenderTargetNeedRedrawListener =
				Infra::EventListener<Render::RenderTarget const *>::bind(
				&Display::__rcmd_onRenderTargetNeedRedraw, this);

			__pRenderTarget = std::unique_ptr<Render::RenderTarget>
			{
				__renderEngine.createRenderTarget(
					hinstance, hwnd, useDepthBuffer, useStencilBuffer)
			};

			__pRenderTarget->getNeedRedrawEvent() += __pRenderTargetNeedRedrawListener;
		}).wait();
	}

	Display::~Display() noexcept
	{
		__rcmdExecutor.run([this]
		{
			__renderEngine.cancelRender(__pRenderTarget.get());
			__pRenderTarget = nullptr;
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
			__renderEngine.reserveRender(__pRenderTarget.get());
		});
	}

	void Display::__rcmd_onRenderTargetNeedRedraw()
	{
		__rcmdExecutor.silentRun([this]
		{
			__renderEngine.reserveRender(__pRenderTarget.get());
		});
	}
}