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
		Render::Engine &renderEngine,
		RendererFactory &rendererFactory)
	{
		__pRcmdExecutor		= &rcmdExecutor;
		__pRenderEngine		= &renderEngine;
		__pRendererFactory	= &rendererFactory;

		__beginningTime = std::chrono::steady_clock::now();

		__modelLoader.filterPrimitiveType(aiPrimitiveType::aiPrimitiveType_POINT, true);
		__modelLoader.filterPrimitiveType(aiPrimitiveType::aiPrimitiveType_LINE, true);

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
		__handleModelRequests();

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

	void Scene::_onModelLoaded(
		uint32_t const requestIdx,
		Model::CreateInfo &&result)
	{}

	Model *Scene::_createModel(
		Model::CreateInfo &&createInfo)
	{
		return new Model
		{
			std::move(createInfo), *__pRcmdExecutor,
			*__pRenderEngine, *__pRendererFactory
		};
	}

	uint32_t Scene::_loadModel(
		std::string_view const &assetPath,
		std::optional<float> const &meshScale)
	{
		uint32_t const reqId{ __modelReqIdAllocator.allocate() };

		std::string modelPath{ assetPath };
		__modelReqMap[reqId] = std::async(std::launch::async, [this, modelPath, meshScale]
		{
			return __modelLoader.load(modelPath, meshScale);
		});

		return reqId;
	}

	Render::Layer *Scene::_rcmd_createLayer()
	{
		return __pRenderEngine->createLayer();
	}

	Render::Mesh *Scene::_rcmd_createMesh()
	{
		return __pRenderEngine->createMesh();
	}

	Render::Texture *Scene::_rcmd_createTexture(
		std::string_view const &assetPath,
		bool const useMipmap,
		VkPipelineStageFlags2 const dstStageMask,
		VkAccessFlags2 const dstAccessMask)
	{
		return TextureUtil::loadTexture(
			*__pRenderEngine, assetPath,
			useMipmap, dstStageMask, dstAccessMask);
	}

	Render::Renderer const *Scene::_rcmd_getRendererOf(
		RendererType const type) const
	{
		return __pRendererFactory->getInstanceOf(type);
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

	void Scene::__handleModelRequests()
	{
		using namespace std::chrono_literals;

		for (auto reqIt{ __modelReqMap.begin() }; reqIt != __modelReqMap.end(); )
		{
			auto &[reqId, reqFuture] { *reqIt };
			if (reqFuture.wait_for(0ns) != std::future_status::ready)
			{
				++reqIt;
				continue;
			}

			__modelReqIdAllocator.free(reqId);
			_onModelLoaded(reqId, reqFuture.get());

			reqIt = __modelReqMap.erase(reqIt);
		}
	}

	void Scene::__rcmd_update(
		std::any const &updateParam)
	{
		_rcmd_onUpdate(updateParam);
		__pRenderEngine->render();
		__rcmdFrameCount.fetch_add(1ULL, std::memory_order::release);
	}
}