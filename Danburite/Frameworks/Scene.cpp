#include "Scene.h"
#include "TextureUtil.h"

namespace Frx
{
	Scene::Scene() noexcept
	{
		setUpdateFrequency(120.0);
	}

	void Scene::init(
		Infra::Executor &rcmdExecutor,
		Render::Engine &renderEngine)
	{
		__pRcmdExecutor = &rcmdExecutor;
		__pRenderEngine = &renderEngine;

		__beginningTime = std::chrono::steady_clock::now();

		auto initParam{ std::move(_onInit()) };
		_rcmd_silentRun([this, initParam{ initParam }]
		{
			_rcmd_onInit(initParam);
		});
	}

	double Scene::getFps() const noexcept
	{
		auto const rcmdFrameCount{ __rcmdFrameCount.load(std::memory_order::acquire) };
		return (static_cast<double>(rcmdFrameCount) / (__time.elapsedTime.count() * 1e-9));
	}

	void Scene::update()
	{
		if (!(__checkFrameDelay()))
			return;

		if (!(__checkUpdateInterval()))
			return;

		__updateTime();
		auto updateParam{ _onUpdate(__time) };
		++__scmdFrameCount;

		_rcmd_silentRun([this, updateParam{ std::move(updateParam) }]
		{
			__rcmd_update(updateParam);
		});
	}

	std::any Scene::_onInit()
	{
		return { };
	}

	std::any Scene::_onUpdate(
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
			TextureUtil::loadTexture(
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
		Infra::Executor::Job &&job)
	{
		return __pRcmdExecutor->run(std::move(job));
	}

	void Scene::_rcmd_silentRun(
		Infra::Executor::Job &&job)
	{
		__pRcmdExecutor->silentRun(std::move(job));
	}

	void Scene::_rcmd_onInit(
		std::any const &initParam)
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

	bool Scene::__checkFrameDelay() const noexcept
	{
		auto const rcmdFrameCount{ __rcmdFrameCount.load(std::memory_order::acquire) };
		return (__scmdFrameCount <= (rcmdFrameCount + __maxFrameDelay));
	}

	bool Scene::__checkUpdateInterval() noexcept
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