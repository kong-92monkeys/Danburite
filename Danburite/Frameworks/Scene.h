#pragma once

#include "../Infra/Executor.h"
#include "../Render/Engine.h"
#include <chrono>

namespace Frx
{
	class Scene : public Infra::Unique
	{
	public:
		struct Time
		{
		public:
			std::chrono::steady_clock::duration elapsedTime{ };
			std::chrono::steady_clock::duration deltaTime{ };
		};

		Scene() noexcept;
		virtual ~Scene() noexcept override;

		void init(
			Infra::Executor &rcmdExecutor,
			Render::Engine &renderEngine);

		constexpr void setUpdateInterval(
			double timeMS) noexcept;

		constexpr void setUpdateFrequency(
			double frequency) noexcept;

	protected:
		virtual void _scmd_onInit();
		virtual void _scmd_onUpdate();

		[[nodiscard]]
		constexpr Time const &_getTime() const noexcept;

		[[nodiscard]]
		constexpr Infra::Executor &_getScmdExecutor() noexcept;

		[[nodiscard]]
		constexpr Infra::Executor &_getRcmdExecutor() const noexcept;

		[[nodiscard]]
		constexpr Render::Engine &_getRenderEngine() const noexcept;

	private:
		Infra::Executor __scmdExecutor;

		std::chrono::time_point<std::chrono::steady_clock> __beginningTime;

		Time __time;

		Infra::Executor *__pRcmdExecutor{ };
		Render::Engine *__pRenderEngine{ };

		std::chrono::steady_clock::duration __updateInterval{ std::chrono::steady_clock::duration::zero() };
		std::chrono::time_point<std::chrono::steady_clock> __lastUpdateTime;

		Infra::EventListenerPtr<Infra::Executor *> __pExecutorIdleListener;

		void __updateTime() noexcept;

		void __scmd_update();
		void __scmd_onIdle();
	};

	constexpr void Scene::setUpdateInterval(
		double const timeMS) noexcept
	{
		__updateInterval = std::chrono::steady_clock::duration{ static_cast<int64_t>(timeMS * 1.0e6) };
	}

	constexpr void Scene::setUpdateFrequency(
		double const frequency) noexcept
	{
		setUpdateInterval(1000.0 / frequency);
	}

	constexpr Scene::Time const &Scene::_getTime() const noexcept
	{
		return __time;
	}

	constexpr Infra::Executor &Scene::_getScmdExecutor() noexcept
	{
		return __scmdExecutor;
	}

	constexpr Infra::Executor &Scene::_getRcmdExecutor() const noexcept
	{
		return *__pRcmdExecutor;
	}

	constexpr Render::Engine &Scene::_getRenderEngine() const noexcept
	{
		return *__pRenderEngine;
	}
}