#include "pch.h"
#include "SceneManager.h"
#include "SceneHandler_Nothing.h"
#include "SceneHandler_PhongTest.h"
#include "SceneHandler_ModelLoading.h"

SceneManager::SceneManager(
	Infra::Executor &uiExecutor,
	Frx::RenderSystem &renderSystem)
{
	__pDisplaySyncListener =
		Infra::EventListener<Frx::Display const *>::bind(
			&SceneManager::__onDisplaySync, this, std::placeholders::_1);

	__scmdExecutor.silentRun([this, &uiExecutor, &renderSystem]
	{
		__scmd_pIdleListener =
			Infra::EventListener<Infra::Looper *>::bind(
				&SceneManager::__scmd_onIdle, this);

		__scmd_handlers[SceneType::NOTHING]			= std::make_unique<SceneHandler_Nothing>();
		__scmd_handlers[SceneType::PHONG_TEST]		= std::make_unique<SceneHandler_PhongTest>();
		__scmd_handlers[SceneType::MODEL_LOADING]	= std::make_unique<SceneHandler_ModelLoading>();

		for (auto const &[_, pHandler] : __scmd_handlers)
			pHandler->init(__scmdExecutor, uiExecutor, renderSystem);

		__scmdExecutor.in_getIdleEvent() += __scmd_pIdleListener;
	});
}

SceneManager::~SceneManager() noexcept
{
	__scmdExecutor.run([this]
	{
		__scmd_pIdleListener = nullptr;
		__scmd_getHandler().deactivate();
		__scmd_handlers.clear();
	}).wait();
}

void SceneManager::registerDisplay(
	Frx::Display *const pDisplay)
{
	if (!(__displays.emplace(pDisplay).second))
		throw std::runtime_error{ "The display is already registered." };

	pDisplay->getSyncEvent() += __pDisplaySyncListener;

	__scmdExecutor.run([this, pDisplay]
	{
		__scmd_getHandler().onDisplayRegistered(pDisplay);
	}).wait();
}

void SceneManager::unregisterDisplay(
	Frx::Display *const pDisplay)
{
	if (!(__displays.erase(pDisplay)))
		throw std::runtime_error{ "The display is not registered yet." };

	pDisplay->getSyncEvent() -= __pDisplaySyncListener;

	__scmdExecutor.run([this, pDisplay]
	{
		__scmd_getHandler().onDisplayUnregistered(pDisplay);
	}).wait();
}

void SceneManager::load(
	SceneType const sceneType)
{
	__scmdExecutor.run([this, sceneType]
	{
		if (__scmd_sceneType == sceneType)
			return;

		__scmd_getHandler().deactivate();
		__scmd_sceneType = sceneType;

		auto &handler{ __scmd_getHandler() };
		handler.activate();

		for (const auto pDisplay : __displays)
			handler.onDisplayRegistered(pDisplay);
	}).wait();
}

void SceneManager::onKeyDown(
	UINT const nChar)
{
	__scmdExecutor.silentRun([this, nChar]
	{
		__scmd_getHandler().onKeyDown(nChar);
	});
}

void SceneManager::onKeyUp(
	UINT const nChar)
{
	__scmdExecutor.silentRun([this, nChar]
	{
		__scmd_getHandler().onKeyUp(nChar);
	});
}

SceneHandler &SceneManager::__scmd_getHandler()
{
	return *(__scmd_handlers.at(__scmd_sceneType));
}

void SceneManager::__onDisplaySync(
	Frx::Display const *const pDisplay)
{
	__scmdExecutor.run([this, pDisplay]
	{
		__scmd_getHandler().onDisplaySync(pDisplay);
	}).wait();
}

void SceneManager::__scmd_onIdle()
{
	__scmd_getHandler().onTick();
}