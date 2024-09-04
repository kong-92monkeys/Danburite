
// ChildView.h : interface of the CMainView class
//


#pragma once


// CMainView window

class CMainView : public CWnd
{
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
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
};

