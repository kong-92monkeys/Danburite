
// App.h : main header file for the App application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "../Infra/Event.h"

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
	constexpr Infra::EventView<> &getIdleEvent() const noexcept;

private:
	mutable Infra::Event<> __idleEvent;
};

extern CApp theApp;

constexpr Infra::EventView<> &CApp::getIdleEvent() const noexcept
{
	return __idleEvent;
}