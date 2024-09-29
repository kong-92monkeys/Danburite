
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
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
};