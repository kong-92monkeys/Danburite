#include "pch.h"
#include "SceneHandler.h"

void SceneHandler::init(
	Frx::RenderSystem &renderSystem)
{
	__pRenderSystem		= &renderSystem;
	_onInit();
}

void SceneHandler::setDisplay(
	Frx::Display *const pDisplay) noexcept
{
	if (__pDisplay == pDisplay)
		return;

	__pDisplay = pDisplay;
	_onDisplayChanged();
}

void SceneHandler::deactivate()
{}

void SceneHandler::activate()
{}

void SceneHandler::onKeyDown(
	UINT const nChar)
{}

void SceneHandler::onKeyUp(
	UINT const nChar)
{}

void SceneHandler::_onInit()
{}

void SceneHandler::_onDisplayChanged()
{}