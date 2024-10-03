#include "pch.h"
#include "SceneHandler_PhongTest.h"
#include "CSceneMenuView01.h"

SceneHandler_PhongTest::SceneHandler_PhongTest() noexcept
{
	__ui_pUIAddLightListener = Infra::EventListener<>::bind(
		&SceneHandler_PhongTest::__ui_onUIAddLight, this);

	__ui_pUIRemoveLightListener = Infra::EventListener<>::bind(
		&SceneHandler_PhongTest::__ui_onUIRemoveLight, this);
}

void SceneHandler_PhongTest::onDisplayRegistered(
	Frx::Display *const pDisplay)
{
	__pScene->setDisplay(pDisplay);
}

void SceneHandler_PhongTest::onDisplayUnregistered(
	Frx::Display *const pDisplay)
{
	if (pDisplay != __pScene->getDisplay())
		return;

	__pScene->setDisplay(nullptr);
}

void SceneHandler_PhongTest::onDisplaySync(
	Frx::Display const *const pDisplay)
{
	if (pDisplay != __pScene->getDisplay())
		return;

	__pScene->syncDisplay();
}

void SceneHandler_PhongTest::onKeyDown(
	UINT const nChar)
{
	switch (nChar)
	{
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

void SceneHandler_PhongTest::onKeyUp(
	UINT const nChar)
{
	switch (nChar)
	{
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

void SceneHandler_PhongTest::onTick()
{
	__pScene->update();

	++__fpsUpdateTick;
	if (__fpsUpdateTick % __fpsUpdateTickCount)
		return;

	__fpsUpdateTick = 0ULL;

	_getUIExecutor().silentRun([this, fps{ __pScene->getFps() }]
	{
		auto const pMenuView{ _ui_getMainFrame()->getSceneMenuView<CSceneMenuView01>() };
		pMenuView->setFPSText(fps);
	});
}

void SceneHandler_PhongTest::_onActivated()
{
	__pScene = _getRenderSystem().createScene<PhongTestScene>();

	_getUIExecutor().silentRun([this]
	{
		auto const pMainFrame{ _ui_getMainFrame() };
		pMainFrame->replaceSceneMenuView<CSceneMenuView01>();

		auto const pMenuView{ pMainFrame->getSceneMenuView<CSceneMenuView01>() };
		pMenuView->getAddLightEvent() += __ui_pUIAddLightListener;
		pMenuView->getRemoveLightEvent() += __ui_pUIRemoveLightListener;
	});
}

void SceneHandler_PhongTest::_onDeactivated()
{
	_getUIExecutor().silentRun([this]
	{
		auto const pMenuView{ _ui_getMainFrame()->getSceneMenuView<CSceneMenuView01>() };
		pMenuView->getAddLightEvent() -= __ui_pUIAddLightListener;
		pMenuView->getRemoveLightEvent() -= __ui_pUIRemoveLightListener;
	});

	__pScene = nullptr;
}

void SceneHandler_PhongTest::__ui_onUIAddLight()
{
	_getScmdExecutor().silentRun([this]
	{
		__pScene->addLight();
	});
}

void SceneHandler_PhongTest::__ui_onUIRemoveLight()
{
	_getScmdExecutor().silentRun([this]
	{
		__pScene->removeLight();
	});
}