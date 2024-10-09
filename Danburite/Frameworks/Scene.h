#pragma once

#include "Model.h"
#include <chrono>
#include <any>
#include "ModelLoader.h"

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
			Render::Engine &renderEngine,
			RendererFactory &rendererFactory);
		
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

		virtual void _onModelLoaded(
			uint32_t requestIdx,
			Model::CreateInfo &&result);

		[[nodiscard]]
		Model *_createModel(
			Model::CreateInfo &&createInfo);

		uint32_t _loadModel(
			std::string_view const &assetPath);

		[[nodiscard]]
		Render::Layer *_rcmd_createLayer();

		[[nodiscard]]
		Render::Mesh *_rcmd_createMesh();

		[[nodiscard]]
		Render::Texture *_rcmd_createTexture(
			std::string_view const &assetPath,
			bool useMipmap,
			VkPipelineStageFlags2 dstStageMask,
			VkAccessFlags2 dstAccessMask);

		template <std::derived_from<Render::Material> $Material, typename ...$Args>
		[[nodiscard]]
		$Material *_rcmd_createMaterial(
			$Args &&...args);

		[[nodiscard]]
		Render::Renderer const *_rcmd_getRendererOf(
			RendererType type) const;

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
		RendererFactory *__pRendererFactory{ };

		uint64_t __maxFrameDelay{ 3ULL };
		uint64_t __scmdFrameCount{ };
		std::atomic_uint64_t __rcmdFrameCount{ };

		std::chrono::steady_clock::duration __updateInterval{ std::chrono::steady_clock::duration::zero() };
		std::chrono::time_point<std::chrono::steady_clock> __lastUpdateTime;

		Infra::IdAllocator<uint32_t> __modelReqIdAllocator;
		std::unordered_map<uint32_t, std::future<Model::CreateInfo>> __modelReqMap;
		ModelLoader __modelLoader;

		void __updateTime() noexcept;

		[[nodiscard]]
		bool __checkFrameDelay() const noexcept;

		[[nodiscard]]
		bool __checkUpdateInterval() noexcept;

		void __handleModelRequests();

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
	$Material *Scene::_rcmd_createMaterial(
		$Args &&...args)
	{
		return __pRenderEngine->createMaterial<$Material>(std::forward<$Args>(args)...);
	}

	template <typename $Data>
	void Scene::_rcmd_setGlobalData(
		$Data const &data) const
	{
		_rcmd_setGlobalData(&data, sizeof($Data));
	}
}