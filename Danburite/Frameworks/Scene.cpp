#include "Scene.h"
#include "../Render/TextureUtil.h"

namespace Frx
{
	Scene::Scene() noexcept
	{
		setUpdateFrequency(60.0);
	}

	void Scene::init(
		Infra::ThreadPool &rcmdExecutor,
		Render::Engine &renderEngine)
	{
		__pRcmdExecutor = &rcmdExecutor;
		__pRenderEngine = &renderEngine;

		_scmd_silentRun([this]
		{
			__pScmdIdleListener =
				Infra::EventListener<Infra::Looper *>::bind(
					&Scene::__scmd_onIdle, this);

			__pScmdExecutor->in_getIdleEvent() += __pScmdIdleListener;
			__beginningTime = std::chrono::steady_clock::now();

			_scmd_onInit();
			_rcmd_silentRun(std::bind(&Scene::_rcmd_onInit, this));
		});
	}

	void Scene::_stopScmdLoop() noexcept
	{
		__pScmdExecutor = nullptr;
	}

	std::unique_ptr<Render::Layer> Scene::_rcmd_createLayer()
	{
		return std::unique_ptr<Render::Layer>{ __pRenderEngine->createLayer() };
	}

	std::unique_ptr<Render::Mesh> Scene::_rcmd_createMesh()
	{
		return std::unique_ptr<Render::Mesh>{ __pRenderEngine->createMesh() };
	}

	std::unique_ptr<Render::Texture> Scene::_rcmd_createTexture(
		std::string_view const &assetPath,
		VkPipelineStageFlags2 const beforeStageMask,
		VkAccessFlags2 const beforeAccessMask,
		VkPipelineStageFlags2 const afterStageMask,
		VkAccessFlags2 const afterAccessMask)
	{
		return std::unique_ptr<Render::Texture>
		{
			Render::TextureUtil::loadTexture(
				*__pRenderEngine, assetPath,
				beforeStageMask, beforeAccessMask,
				afterStageMask, afterAccessMask)
		};
	}

	std::future<void> Scene::_scmd_run(
		Infra::ThreadPool::Job &&job)
	{
		return __pScmdExecutor->run(std::move(job));
	}

	void Scene::_scmd_silentRun(
		Infra::ThreadPool::Job &&job)
	{
		__pScmdExecutor->silentRun(std::move(job));
	}

	std::future<void> Scene::_rcmd_run(
		Infra::ThreadPool::Job &&job)
	{
		return __pRcmdExecutor->run(std::move(job));
	}

	void Scene::_rcmd_silentRun(
		Infra::ThreadPool::Job &&job)
	{
		__pRcmdExecutor->silentRun(std::move(job));
	}

	void Scene::_scmd_onInit()
	{}

	std::any Scene::_scmd_onUpdate(
		Time const &time)
	{
		return { };
	}

	void Scene::_rcmd_onInit()
	{}

	void Scene::_rcmd_onUpdate(
		std::any const &updateParam)
	{}

	void Scene::__updateTime() noexcept
	{
		auto const curElapsedTime{ std::chrono::steady_clock::now() - __beginningTime };

		if (__time.elapsedTime.count() > 0)
			__time.deltaTime = (curElapsedTime - __time.elapsedTime);

		__time.elapsedTime = curElapsedTime;
	}

	std::any Scene::__scmd_update()
	{
		__updateTime();
		auto retVal{ _scmd_onUpdate(__time) };
		++__scmdFrameCount;

		return retVal;
	}

	void Scene::__rcmd_update(
		std::any const &updateParam)
	{
		_rcmd_onUpdate(updateParam);
		__pRenderEngine->render();
		__rcmdFrameCount.fetch_add(1ULL, std::memory_order::release);
	}

	void Scene::__scmd_onIdle()
	{
		if (!(__scmd_checkFrameDelay()))
			return;

		if (!(__scmd_checkUpdateInterval()))
			return;
		
		auto updateParam{ __scmd_update() };
		_rcmd_silentRun([this, updateParam{ std::move(updateParam) }]
		{
			__rcmd_update(updateParam);
		});
	}

	bool Scene::__scmd_checkFrameDelay() const noexcept
	{
		auto const rcmdFrameCount{ __rcmdFrameCount.load(std::memory_order::acquire) };
		return (__scmdFrameCount <= (rcmdFrameCount + __maxFrameDelay));
	}

	bool Scene::__scmd_checkUpdateInterval() noexcept
	{
		auto const curTime{ std::chrono::steady_clock::now() };
		if (__updateInterval >= (curTime - __lastUpdateTime))
			return false;

		__lastUpdateTime = curTime;
		return true;
	}
}