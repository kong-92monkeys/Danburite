#pragma once

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
			Infra::Executor &rcmdExecutor,
			Render::Engine &renderEngine);
		
		[[nodiscard]]
		double getFps() const noexcept;

		constexpr void setMaxFrameDelay(
			uint64_t maxDelay);

		constexpr void setUpdateInterval(
			double timeMS) noexcept;

		constexpr void setUpdateFrequency(
			double frequency) noexcept;

		void update();

	protected:
		[[nodiscard]]
		virtual std::any _onInit();

		[[nodiscard]]
		virtual std::any _onUpdate(
			Time const &time);

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

		void _rcmd_setGlobalData(
			void const *pData,
			size_t size) const;

		template <typename $Data>
		void _rcmd_setGlobalData(
			$Data const &data) const;

		void _rcmd_addGlobalMaterial(
			Render::Material const *pMaterial) const;

		void _rcmd_removeGlobalMaterial(
			Render::Material const *pMaterial) const;

		[[nodiscard]]
		uint32_t _rcmd_getGlobalMaterialIdOf(
			Render::Material const *pMaterial) const;

		[[nodiscard]]
		std::future<void> _rcmd_run(
			Infra::Executor::Job &&job);

		void _rcmd_silentRun(
			Infra::Executor::Job &&job);

		virtual void _rcmd_onInit(
			std::any const &initParam);

		virtual void _rcmd_onUpdate(
			std::any const &updateParam);

	private:
		std::chrono::time_point<std::chrono::steady_clock> __beginningTime;

		Time __time;

		Infra::Executor *__pRcmdExecutor{ };
		Render::Engine *__pRenderEngine{ };

		uint64_t __maxFrameDelay{ 3ULL };
		uint64_t __scmdFrameCount{ };
		std::atomic_uint64_t __rcmdFrameCount{ };

		std::chrono::steady_clock::duration __updateInterval{ std::chrono::steady_clock::duration::zero() };
		std::chrono::time_point<std::chrono::steady_clock> __lastUpdateTime;

		void __updateTime() noexcept;

		[[nodiscard]]
		bool __checkFrameDelay() const noexcept;

		[[nodiscard]]
		bool __checkUpdateInterval() noexcept;

		void __rcmd_update(
			std::any const &updateParam);
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

	template <typename $Data>
	void Scene::_rcmd_setGlobalData(
		$Data const &data) const
	{
		_rcmd_setGlobalData(&data, sizeof($Data));
	}
}