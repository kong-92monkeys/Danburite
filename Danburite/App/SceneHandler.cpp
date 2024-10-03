#include "pch.h"
#include "SceneHandler.h"

void SceneHandler::init(
	Infra::Executor &scmdExecutor,
	Infra::Executor &uiExecutor,
	Frx::RenderSystem &renderSystem)
{
	__pScmdExecutor		= &scmdExecutor;
	__pUIExecutor		= &uiExecutor;
	__pRenderSystem		= &renderSystem;
	_onInit();
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

void SceneHandler::onDisplayRegistered(
	Frx::Display *const pDisplay)
{}

void SceneHandler::onDisplayUnregistered(
	Frx::Display *const pDisplay)
{}

void SceneHandler::onDisplaySync(
	Frx::Display const *const pDisplay)
{}

void SceneHandler::onKeyDown(
	UINT const nChar)
{}

void SceneHandler::onKeyUp(
	UINT const nChar)
{}

void SceneHandler::onTick()
{}

void SceneHandler::_onInit()
{}

void SceneHandler::_onActivated()
{}

void SceneHandler::_onDeactivated()
{}

CMainFrame *SceneHandler::_ui_getMainFrame() const
{
	return STATIC_DOWNCAST(CMainFrame, AfxGetApp()->GetMainWnd());
}