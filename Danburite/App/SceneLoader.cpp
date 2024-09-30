#include "pch.h"
#include "SceneLoader.h"
#include "SceneHandler_Nothing.h"

SceneLoader::SceneLoader(
	Frx::RenderSystem &renderSystem) :
	__renderSystem	{ renderSystem }
{
	__sceneHandlers[SceneType::NOTHING] = std::make_unique<SceneHandler_Nothing>();

	for (auto const &[type, pHandler] : __sceneHandlers)
		pHandler->init(__renderSystem);
}

void SceneLoader::load(
	SceneType const sceneType)
{
	__sceneHandlers.at(__sceneType)->deactivate();
	__sceneType = sceneType;

	auto &sceneHandler{ *(__sceneHandlers.at(sceneType)) };
	sceneHandler.activate();
}

void SceneLoader::setDisplay(
	Frx::Display *const pDisplay) noexcept
{
	auto &sceneHandler{ *(__sceneHandlers.at(__sceneType)) };
	sceneHandler.setDisplay(pDisplay);
}

void SceneLoader::onViewKeyDown(
	UINT const nChar)
{
	auto &sceneHandler{ *(__sceneHandlers.at(__sceneType)) };
	sceneHandler.onKeyDown(nChar);
}

void SceneLoader::onViewKeyUp(
	UINT const nChar)
{
	auto &sceneHandler{ *(__sceneHandlers.at(__sceneType)) };
	sceneHandler.onKeyUp(nChar);
}