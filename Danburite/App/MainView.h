
// ChildView.h : interface of the CMainView class
//

#pragma once

#include "../Frameworks/Display.h"

// CMainView window

class CMainView : public CWnd
{
	DECLARE_DYNCREATE(CMainView)

// Construction
public:
	CMainView();

// Attributes
public:

// Operations
public:
	void enableDisplay(
		bool enabled);

	[[nodiscard]]
	constexpr Frx::Display *getDisplay() noexcept;

// Overrides
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainView();

	// Generated message map functions
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC *pDC);

protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()

private:
	std::unique_ptr<Frx::Display> __pDisplay;

	bool __displayEnabled{ false };
};

constexpr Frx::Display *CMainView::getDisplay() noexcept
{
	return __pDisplay.get();
}