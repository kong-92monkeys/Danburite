#include "pch.h"
#include "SceneLoader.h"
#include "SceneHandler_Nothing.h"
#include "SceneHandler_PhongTest.h"

SceneLoader::SceneLoader(
	Frx::RenderSystem &renderSystem) :
	__renderSystem	{ renderSystem }
{
	__sceneHandlers[SceneType::NOTHING]		= std::make_unique<SceneHandler_Nothing>();
	__sceneHandlers[SceneType::PHONG_TEST]	= std::make_unique<SceneHandler_PhongTest>();

	for (auto const &[_, pHandler] : __sceneHandlers)
		pHandler->init(__renderSystem);
}

void SceneLoader::load(
	SceneType const sceneType)
{
	if (__sceneType == sceneType)
		return;

	__sceneHandlers.at(__sceneType)->deactivate();
	__sceneType = sceneType;

	auto &sceneHandler{ *(__sceneHandlers.at(sceneType)) };
	sceneHandler.activate();
}

void SceneLoader::setDisplay(
	Frx::Display *const pDisplay) noexcept
{
	for (auto const &[_, pHandler] : __sceneHandlers)
		pHandler->setDisplay(pDisplay);
}

void SceneLoader::onKeyDown(
	UINT const nChar)
{
	auto &sceneHandler{ *(__sceneHandlers.at(__sceneType)) };
	sceneHandler.onKeyDown(nChar);
}

void SceneLoader::onKeyUp(
	UINT const nChar)
{
	auto &sceneHandler{ *(__sceneHandlers.at(__sceneType)) };
	sceneHandler.onKeyUp(nChar);
}