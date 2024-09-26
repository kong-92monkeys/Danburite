
// App.h : main header file for the App application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "../Frameworks/RenderSystem.h"
#include "PhongTestScene.h"

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
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

public:
	[[nodiscard]]
	std::unique_ptr<Frx::Display> createDisplay(
		HWND hwnd);

	void setSceneDisplay(
		Frx::Display *pDisplay);

	[[nodiscard]]
	constexpr Infra::EventView<> &getIdleEvent() const noexcept;

private:
	std::unique_ptr<Dev::Context> __pVulkanContext;
	std::unique_ptr<Frx::RenderSystem> __pRenderSystem;
	std::unique_ptr<PhongTestScene> __pScene;

	mutable Infra::Event<> __idleEvent;

	void __onInitBeforeMainFrame();
};

constexpr Infra::EventView<> &CApp::getIdleEvent() const noexcept
{
	return __idleEvent;
}

extern CApp theApp;