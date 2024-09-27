#pragma once

#include "../Infra/Looper.h"
#include "../Render/Engine.h"
#include <chrono>
#include <any>

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
		virtual ~Scene() noexcept override = default;

		void init(
			Infra::ThreadPool &rcmdExecutor,
			Render::Engine &renderEngine);

		constexpr void setMaxFrameDelay(
			uint64_t maxDelay);

		constexpr void setUpdateInterval(
			double timeMS) noexcept;

		constexpr void setUpdateFrequency(
			double frequency) noexcept;

	protected:
		void _stopLoop() noexcept;

		[[nodiscard]]
		double _scmd_getFps() const noexcept;

		[[nodiscard]]
		std::unique_ptr<Render::Layer> _rcmd_createLayer();

		[[nodiscard]]
		std::unique_ptr<Render::Mesh> _rcmd_createMesh();

		[[nodiscard]]
		std::unique_ptr<Render::Texture> _rcmd_createTexture(
			std::string_view const &assetPath,
			VkPipelineStageFlags2 beforeStageMask,
			VkAccessFlags2 beforeAccessMask,
			VkPipelineStageFlags2 afterStageMask,
			VkAccessFlags2 afterAccessMask);

		template <std::derived_from<Render::Material> $Material, typename ...$Args>
		[[nodiscard]]
		std::unique_ptr<$Material> _rcmd_createMaterial($Args &&...args);

		template <std::derived_from<Render::Renderer> $Renderer, typename ...$Args>
		[[nodiscard]]
		std::unique_ptr<$Renderer> _rcmd_createRenderer($Args &&...args);

		[[nodiscard]]
		std::future<void> _scmd_run(
			Infra::ThreadPool::Job &&job);

		void _scmd_silentRun(
			Infra::ThreadPool::Job &&job);

		[[nodiscard]]
		std::future<void> _rcmd_run(
			Infra::ThreadPool::Job &&job);

		void _rcmd_silentRun(
			Infra::ThreadPool::Job &&job);

		virtual void _scmd_onInit();

		[[nodiscard]]
		virtual std::any _scmd_onUpdate(
			Time const &time);

		virtual void _rcmd_onInit();
		virtual void _rcmd_onUpdate(
			std::any const &updateParam);

	private:
		std::unique_ptr<Infra::Looper> __pScmdExecutor{ std::make_unique<Infra::Looper>() };

		std::chrono::time_point<std::chrono::steady_clock> __beginningTime;

		Time __time;

		Infra::ThreadPool *__pRcmdExecutor{ };
		Render::Engine *__pRenderEngine{ };

		uint64_t __maxFrameDelay{ 3ULL };
		uint64_t __scmdFrameCount{ };
		std::atomic_uint64_t __rcmdFrameCount{ };

		std::chrono::steady_clock::duration __updateInterval{ std::chrono::steady_clock::duration::zero() };
		std::chrono::time_point<std::chrono::steady_clock> __lastUpdateTime;

		Infra::EventListenerPtr<Infra::Looper *> __pScmdIdleListener;

		void __updateTime() noexcept;

		[[nodiscard]]
		std::any __scmd_update();
		void __rcmd_update(
			std::any const &updateParam);

		void __scmd_onIdle();

		[[nodiscard]]
		bool __scmd_checkFrameDelay() const noexcept;

		[[nodiscard]]
		bool __scmd_checkUpdateInterval() noexcept;
	};

	constexpr void Scene::setMaxFrameDelay(
		uint64_t const maxDelay)
	{
		__maxFrameDelay = maxDelay;
	}

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

	template <std::derived_from<Render::Material> $Material, typename ...$Args>
	std::unique_ptr<$Material> Scene::_rcmd_createMaterial($Args &&...args)
	{
		return std::unique_ptr<$Material>
		{
			__pRenderEngine->createMaterial<$Material>(
				std::forward<$Args>(args)...)
		};
	}

	template <std::derived_from<Render::Renderer> $Renderer, typename ...$Args>
	std::unique_ptr<$Renderer> Scene::_rcmd_createRenderer($Args &&...args)
	{
		return std::unique_ptr<$Renderer>
		{
			__pRenderEngine->createRenderer<$Renderer>(
				std::forward<$Args>(args)...)
		};
	}
}