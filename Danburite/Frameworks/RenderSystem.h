#pragma once

#include "../Infra/SingleThreadPool.h"
#include "Display.h"
#include "Scene.h"
#include "Model.h"
#include <limits>

namespace Frx
{
	class RenderSystem : public Infra::Unique
	{
	public:
		RenderSystem(
			Dev::Context &context,
			VK::PhysicalDevice const &physicalDevice);

		virtual ~RenderSystem() noexcept override;

		[[nodiscard]]
		Display *createDisplay(
			HINSTANCE hinstance,
			HWND hwnd,
			bool useDepthBuffer,
			bool useStencilBuffer);

		template <std::derived_from<Scene> $Scene, typename ...$Args>
		[[nodiscard]]
		$Scene *createScene($Args &&...args);

	private:
		Infra::SingleThreadPool __rcmdExecutor;

		std::array<std::byte, sizeof(Render::Engine)> __enginePlaceholder{ };
		std::array<std::byte, sizeof(RendererFactory)> __rendererFactoryPlaceholder{ };

		void __createEngine(
			Dev::Context &context,
			VK::PhysicalDevice const &physicalDevice);

		void __createRenderFactory();

		[[nodiscard]]
		Render::Engine &__getRenderEngine() noexcept;

		[[nodiscard]]
		RendererFactory &__getRendererFactory() noexcept;
	};

	template <std::derived_from<Scene> $Scene, typename ...$Args>
	$Scene *RenderSystem::createScene($Args &&...args)
	{
		auto pScene{ new $Scene{ std::forward<$Args>(args)... } };
		pScene->init(__rcmdExecutor, __getRenderEngine(), __getRendererFactory());
		return pScene;
	}
}