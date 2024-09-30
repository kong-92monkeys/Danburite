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

	if (__activated)
		_onDisplayChanged(pDisplay);
}

void SceneHandler::activate()
{
	if (__activated)
		return;

	__activated = true;
	_onActivated();
}

void SceneHandler::deactivate()
{
	if (!__activated)
		return;

	__activated = false;
	_onDeactivated();
}

void SceneHandler::onKeyDown(
	UINT const nChar)
{}

void SceneHandler::onKeyUp(
	UINT const nChar)
{}

void SceneHandler::_onInit()
{}

void SceneHandler::_onActivated()
{}

void SceneHandler::_onDeactivated()
{}

void SceneHandler::_onDisplayChanged(
	Frx::Display *const pDisplay)
{}

CMainFrame *SceneHandler::_getMainFrame() const
{
	return STATIC_DOWNCAST(CMainFrame, AfxGetApp()->GetMainWnd());
}