
// App.h : main header file for the App application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "../Frameworks/RenderSystem.h"
#include "MainView.h"
#include "SceneManager.h"
#include "../Infra/JobPipe.h"

// CApp:
// See App.cpp for the implementation of this class
//

class CApp : public CWinApp
{
public:
	CApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);

// Implementation

public:
	afx_msg void OnScenes00();
	afx_msg void OnScenes01();
	afx_msg void OnAppAbout();

	DECLARE_MESSAGE_MAP()

public:
	[[nodiscard]]
	std::unique_ptr<Frx::Display> createDisplay(
		HWND hwnd,
		bool useDepthBuffer,
		bool useStencilBuffer);

	void setMainView(
		CMainView *pMainView);

	void onKeyDown(UINT nChar);
	void onKeyUp(UINT nChar);

	[[nodiscard]]
	constexpr Infra::Executor &getUIExecutor() noexcept;

	[[nodiscard]]
	constexpr Infra::EventView<> &getUIIdleEvent() const noexcept;

private:
	Infra::JobPipe __uiExecutor;
	std::vector<Infra::JobPipe::JobInfo> __uiJobs;

	std::unique_ptr<Dev::Context> __pVulkanContext;
	std::unique_ptr<Frx::RenderSystem> __pRenderSystem;
	std::unique_ptr<SceneManager> __pSceneManager;

	CMainView *__pMainView{ };

	mutable Infra::Event<> __idleEvent;

	void __onInitBeforeMainFrame();

	void __loadScene(
		SceneType sceneType);
};

constexpr Infra::Executor &CApp::getUIExecutor() noexcept
{
	return __uiExecutor;
}

constexpr Infra::EventView<> &CApp::getUIIdleEvent() const noexcept
{
	return __idleEvent;
}

extern CApp theApp;