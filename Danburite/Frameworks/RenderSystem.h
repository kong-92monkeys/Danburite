#pragma once

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

		constexpr void setFps(
			double fps) noexcept;

		[[nodiscard]]
		std::unique_ptr<Display> createDisplay(
			HINSTANCE hinstance,
			HWND hwnd);

		template <std::derived_from<Scene> $Scene, typename ...$Args>
		[[nodiscard]]
		std::unique_ptr<$Scene> createScene($Args &&...args);

	private:
		Infra::Executor __rcmdExecutor;
		std::array<std::byte, sizeof(Render::Engine)> __enginePlaceholder{ };

		std::chrono::steady_clock::duration __frameTime{ std::chrono::steady_clock::duration::zero() };
		std::chrono::time_point<std::chrono::steady_clock> __lastRenderTime;

		Infra::EventListenerPtr<Infra::Executor *> __pExecutorIdleListener;

		void __createEngine(
			Dev::Context &context,
			VK::PhysicalDevice const &physicalDevice);

		void __rcmd_onIdle();

		[[nodiscard]]
		Render::Engine &__getRenderEngine() noexcept;
	};

	constexpr void RenderSystem::setFps(
		double const fps) noexcept
	{
		__frameTime = std::chrono::steady_clock::duration{ static_cast<int64_t>(1.0e9 / fps) };
	}

	template <std::derived_from<Scene> $Scene, typename ...$Args>
	std::unique_ptr<$Scene> RenderSystem::createScene($Args &&...args)
	{
		auto pScene{ std::make_unique<$Scene>(std::forward<$Args>(args)...) };
		pScene->init(__rcmdExecutor, __getRenderEngine());
		return pScene;
	}
}