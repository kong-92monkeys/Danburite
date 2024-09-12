
// ChildView.h : interface of the CMainView class
//

#pragma once

#include "../Render/RenderTarget.h"

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

protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()

private:
	std::unique_ptr<Render::RenderTarget> __pRenderTarget;
};

