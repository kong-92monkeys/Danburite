#include "Scene.h"
#include "../Render/TextureUtil.h"

namespace Frx
{
	Scene::Scene() noexcept
	{
		setUpdateFrequency(120.0);
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
			__scmd_beginningTime = std::chrono::steady_clock::now();

			auto initParam{ _scmd_onInit() };
			_rcmd_silentRun([this, initParam{ std::move(initParam) }]
			{
				_rcmd_onInit(initParam);
			});
		});
	}

	double Scene::getFps() const noexcept
	{
		return __fps.load(std::memory_order::acquire);
	}

	void Scene::_stopLoop() noexcept
	{
		__pScmdExecutor = nullptr;
	}

	double Scene::_scmd_calcFps() const noexcept
	{
		auto const rcmdFrameCount{ __rcmdFrameCount.load(std::memory_order::acquire) };
		return (static_cast<double>(rcmdFrameCount) / (__scmd_time.elapsedTime.count() * 1e-9));
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

	std::any Scene::_scmd_onInit()
	{
		return { };
	}

	std::any Scene::_scmd_onUpdate(
		Time const &time)
	{
		return { };
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

	void Scene::_rcmd_setGlobalData(
		void const *const pData,
		size_t const size) const
	{
		__pRenderEngine->setGlobalData(pData, size);
	}

	void Scene::_rcmd_addGlobalMaterial(
		Render::Material const *const pMaterial) const
	{
		return __pRenderEngine->addGlobalMaterial(pMaterial);
	}

	void Scene::_rcmd_removeGlobalMaterial(
		Render::Material const *const pMaterial) const
	{
		return __pRenderEngine->removeGlobalMaterial(pMaterial);
	}

	uint32_t Scene::_rcmd_getGlobalMaterialIdOf(
		Render::Material const *const pMaterial) const
	{
		return __pRenderEngine->getGlobalMaterialIdOf(pMaterial);
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

	void Scene::_rcmd_onInit(
		std::any const &initParam)
	{}

	void Scene::_rcmd_onUpdate(
		std::any const &updateParam)
	{}

	void Scene::__scmd_updateTime() noexcept
	{
		auto const curElapsedTime{ std::chrono::steady_clock::now() - __scmd_beginningTime };

		if (__scmd_time.elapsedTime.count() > 0)
			__scmd_time.deltaTime = (curElapsedTime - __scmd_time.elapsedTime);

		__scmd_time.elapsedTime = curElapsedTime;
	}

	std::any Scene::__scmd_update()
	{
		__scmd_updateTime();
		__fps.store(_scmd_calcFps(), std::memory_order::release);

		auto retVal{ _scmd_onUpdate(__scmd_time) };
		++__scmdFrameCount;

		return retVal;
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

	void Scene::__rcmd_update(
		std::any const &updateParam)
	{
		_rcmd_onUpdate(updateParam);
		__pRenderEngine->render();
		__rcmdFrameCount.fetch_add(1ULL, std::memory_order::release);
	}
}