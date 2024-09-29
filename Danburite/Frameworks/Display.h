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
			bool useDepthBuffer,
			bool useStencilBuffer);

		virtual ~Display() noexcept override;

		[[nodiscard]]
		constexpr uint32_t getWidth() const noexcept;

		[[nodiscard]]
		constexpr uint32_t getHeight() const noexcept;

		void sync();
		void draw();

		[[nodiscard]]
		constexpr Render::RenderTarget &rcmd_getRenderTarget() noexcept;

	private:
		Infra::ThreadPool &__rcmdExecutor;
		Render::Engine &__renderEngine;

		std::unique_ptr<Render::RenderTarget> __pRenderTarget;

		uint32_t __width{ };
		uint32_t __height{ };

		Infra::EventListenerPtr<Render::RenderTarget const *> __pRenderTargetNeedRedrawListener;

		void __rcmd_onRenderTargetNeedRedraw();
	};

	constexpr uint32_t Display::getWidth() const noexcept
	{
		return __width;
	}

	constexpr uint32_t Display::getHeight() const noexcept
	{
		return __height;
	}

	constexpr Render::RenderTarget &Display::rcmd_getRenderTarget() noexcept
	{
		return *__pRenderTarget;
	}
}
