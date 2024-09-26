#include "Scene.h"

namespace Frx
{
	Scene::Scene() noexcept
	{
		setUpdateFrequency(60.0);
	}

	Scene::~Scene() noexcept
	{
		__scmdExecutor.run([this]
		{
			if (!__pExecutorIdleListener)
				return;

			__scmdExecutor.exec_getIdleEvent() -= __pExecutorIdleListener;
		}).wait();
	}

	void Scene::init(
		Infra::Executor &rcmdExecutor,
		Render::Engine &renderEngine)
	{
		__pRcmdExecutor = &rcmdExecutor;
		__pRenderEngine = &renderEngine;

		__scmdExecutor.silentRun([this]
		{
			__pExecutorIdleListener =
				Infra::EventListener<Infra::Executor *>::bind(
					&Scene::__scmd_onIdle, this);

			__scmdExecutor.exec_getIdleEvent() += __pExecutorIdleListener;

			__beginningTime = std::chrono::steady_clock::now();
			_scmd_onInit();
		});
	}

	void Scene::_scmd_onInit()
	{}

	void Scene::_scmd_onUpdate()
	{}

	void Scene::__updateTime() noexcept
	{
		auto const curElapsedTime{ std::chrono::steady_clock::now() - __beginningTime };

		if (__time.elapsedTime.count() > 0)
			__time.deltaTime = (curElapsedTime - __time.elapsedTime);

		__time.elapsedTime = curElapsedTime;
	}

	void Scene::__scmd_update()
	{
		__updateTime();
		_scmd_onUpdate();
	}

	void Scene::__scmd_onIdle()
	{
		auto const curTime{ std::chrono::steady_clock::now() };
		if (__updateInterval >= (curTime - __lastUpdateTime))
			return;

		__lastUpdateTime = curTime;
		__scmd_update();
	}
}