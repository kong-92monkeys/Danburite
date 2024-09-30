#include "pch.h"
#include "SceneHandler_Nothing.h"
#include "CSceneMenuView00.h"

void SceneHandler_Nothing::_onActivated()
{
	_getMainFrame()->replaceSceneMenuView<CSceneMenuView00>();
}