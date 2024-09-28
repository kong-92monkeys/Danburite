#pragma once

#include "../Infra/ThreadPool.h"
#include "../Render/Engine.h"

namespace Frx
{
	class Display : public Infra::Unique
	{
	public:
		Display(
			Infra::ThreadPool &rcmdExecutor,
			Render::Engine &renderEngine,
			HINSTANCE hinstance,
			HWND hwnd,
			bool useDepthStencilBuffer);

		virtual ~Display() noexcept override;

		void sync();
		void draw();

		[[nodiscard]]
		constexpr Render::RenderTarget &rcmd_getRenderTarget() noexcept;

	private:
		Infra::ThreadPool &__rcmdExecutor;
		Render::Engine &__renderEngine;

		std::unique_ptr<Render::RenderTarget> __pRenderTarget;
		Infra::EventListenerPtr<Render::RenderTarget const *> __pRenderTargetNeedRedrawListener;

		void __rcmd_onRenderTargetNeedRedraw();
	};

	constexpr Render::RenderTarget &Display::rcmd_getRenderTarget() noexcept
	{
		return *__pRenderTarget;
	}
}
