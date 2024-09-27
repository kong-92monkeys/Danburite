#pragma once

#include "../Infra/SingleThreadPool.h"
#include "Display.h"
#include "Scene.h"
#include <limits>
#include <chrono>

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
		std::unique_ptr<Display> createDisplay(
			HINSTANCE hinstance,
			HWND hwnd);

		template <std::derived_from<Scene> $Scene, typename ...$Args>
		[[nodiscard]]
		std::unique_ptr<$Scene> createScene($Args &&...args);

	private:
		Infra::SingleThreadPool __rcmdExecutor;
		std::array<std::byte, sizeof(Render::Engine)> __enginePlaceholder{ };

		void __createEngine(
			Dev::Context &context,
			VK::PhysicalDevice const &physicalDevice);

		[[nodiscard]]
		Render::Engine &__getRenderEngine() noexcept;
	};

	template <std::derived_from<Scene> $Scene, typename ...$Args>
	std::unique_ptr<$Scene> RenderSystem::createScene($Args &&...args)
	{
		auto pScene{ std::make_unique<$Scene>(std::forward<$Args>(args)...) };
		pScene->init(__rcmdExecutor, __getRenderEngine());
		return pScene;
	}
}