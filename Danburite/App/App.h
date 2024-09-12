
// App.h : main header file for the App application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "../Infra/Event.h"
#include "../Render/Engine.h"

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
	constexpr Render::Engine &getRenderEngine() noexcept;

	[[nodiscard]]
	constexpr Infra::EventView<> &getIdleEvent() const noexcept;

private:
	std::unique_ptr<Dev::Context> __pVulkanContext;
	std::unique_ptr<Render::Engine> __pRenderEngine;

	mutable Infra::Event<> __idleEvent;

	void __onInitBeforeMainFrame();
};

extern CApp theApp;

constexpr Render::Engine &CApp::getRenderEngine() noexcept
{
	return *__pRenderEngine;
}

constexpr Infra::EventView<> &CApp::getIdleEvent() const noexcept
{
	return __idleEvent;
}