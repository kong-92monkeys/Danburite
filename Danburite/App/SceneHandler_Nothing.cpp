#include "pch.h"
#include "SceneHandler_Nothing.h"
#include "CSceneMenuView00.h"

void SceneHandler_Nothing::_onActivated()
{
	_getUIExecutor().silentRun([this]
	{
		_ui_getMainFrame()->replaceSceneMenuView<CSceneMenuView00>();
	});
}