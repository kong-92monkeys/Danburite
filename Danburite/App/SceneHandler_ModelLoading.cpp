#include "pch.h"
#include "SceneHandler_ModelLoading.h"
#include "CSceneMenuView02.h"

void SceneHandler_ModelLoading::onDisplayRegistered(
	Frx::Display *const pDisplay)
{
	__pScene->setDisplay(pDisplay);
}

void SceneHandler_ModelLoading::onDisplayUnregistered(
	Frx::Display *const pDisplay)
{
	if (pDisplay != __pScene->getDisplay())
		return;

	__pScene->setDisplay(nullptr);
}

void SceneHandler_ModelLoading::onDisplaySync(
	Frx::Display const *const pDisplay)
{
	if (pDisplay != __pScene->getDisplay())
		return;

	__pScene->syncDisplay();
}

void SceneHandler_ModelLoading::onKeyDown(
	UINT const nChar)
{
	switch (nChar)
	{
		case VK_SHIFT:
			__pScene->startCameraMoveAcceleration();
			break;

		case 'd':
		case 'D':
			__pScene->startCameraMoveRight();
			break;

		case 'a':
		case 'A':
			__pScene->startCameraMoveLeft();
			break;

		case 'e':
		case 'E':
			__pScene->startCameraMoveUp();
			break;

		case 'q':
		case 'Q':
			__pScene->startCameraMoveDown();
			break;

		case 'w':
		case 'W':
			__pScene->startCameraMoveForward();
			break;

		case 's':
		case 'S':
			__pScene->startCameraMoveBackward();
			break;

		case VK_RIGHT:
			__pScene->startCameraRotateRight();
			break;

		case VK_LEFT:
			__pScene->startCameraRotateLeft();
			break;

		case VK_UP:
			__pScene->startCameraRotateUp();
			break;

		case VK_DOWN:
			__pScene->startCameraRotateDown();
			break;
	}
}

void SceneHandler_ModelLoading::onKeyUp(
	UINT const nChar)
{
	switch (nChar)
	{
		case VK_SHIFT:
			__pScene->endCameraMoveAcceleration();
			break;

		case 'd':
		case 'D':
			__pScene->endCameraMoveRight();
			break;

		case 'a':
		case 'A':
			__pScene->endCameraMoveLeft();
			break;

		case 'e':
		case 'E':
			__pScene->endCameraMoveUp();
			break;

		case 'q':
		case 'Q':
			__pScene->endCameraMoveDown();
			break;

		case 'w':
		case 'W':
			__pScene->endCameraMoveForward();
			break;

		case 's':
		case 'S':
			__pScene->endCameraMoveBackward();
			break;

		case VK_RIGHT:
			__pScene->endCameraRotateRight();
			break;

		case VK_LEFT:
			__pScene->endCameraRotateLeft();
			break;

		case VK_UP:
			__pScene->endCameraRotateUp();
			break;

		case VK_DOWN:
			__pScene->endCameraRotateDown();
			break;
	}
}

void SceneHandler_ModelLoading::onTick()
{
	__pScene->update();

	++__fpsUpdateTick;
	if (__fpsUpdateTick % __fpsUpdateTickCount)
		return;

	__fpsUpdateTick = 0ULL;

	_getUIExecutor().silentRun([this, fps{ __pScene->getFps() }]
	{
		auto const pMenuView{ _ui_getMainFrame()->getSceneMenuView<CSceneMenuView02>() };
		pMenuView->setFPSText(fps);
	});
}

void SceneHandler_ModelLoading::_onActivated()
{
	__pScene = std::unique_ptr<ModelLoadingScene>{ _getRenderSystem().createScene<ModelLoadingScene>() };

	_getUIExecutor().silentRun([this]
	{
		auto const pMainFrame{ _ui_getMainFrame() };
		pMainFrame->replaceSceneMenuView<CSceneMenuView02>();
	});
}

void SceneHandler_ModelLoading::_onDeactivated()
{
	__pScene = nullptr;
}