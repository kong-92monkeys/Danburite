
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "pch.h"
#include "framework.h"
#include "App.h"

#include "MainFrm.h"
#include "MainView.h"
#include "LogView.h"
#include "CSceneMenuView00.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "CSceneMenuView01.h"

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
END_MESSAGE_MAP()

// CMainFrame construction/destruction

CMainFrame::CMainFrame() noexcept
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.cx = 1920;
	cs.cy = 1080;
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);

	return TRUE;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext *pContext)
{
	// TODO: Add your specialized code here and/or call the base class
	if (!(__windowSplitter.CreateStatic(this, 1, 3)))
	{
		TRACE("Cannot split the window.");
		return FALSE;
	}

	if (!(__windowSplitter.CreateView(0, 0, RUNTIME_CLASS(CLogView), CSize{ 400, 0 }, pContext)))
	{
		TRACE("Cannot create the log view.");
		return FALSE;
	}

	if (!(__windowSplitter.CreateView(0, 1, RUNTIME_CLASS(CSceneMenuView00), CSize{ 400, 0 }, pContext)))
	{
		TRACE("Cannot create the menu view.");
		return FALSE;
	}

	if (!(__windowSplitter.CreateView(0, 2, RUNTIME_CLASS(CMainView), CSize{ 0, 0 }, pContext)))
	{
		TRACE("Cannot create the main view.");
		return FALSE;
	}

	auto const pLogView{ STATIC_DOWNCAST(CLogView, __windowSplitter.GetPane(0, 0)) };
	pLogView->emplaceLoggerImpl();

	return CFrameWnd::OnCreateClient(lpcs, pContext);
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

BOOL CMainFrame::PreTranslateMessage(MSG *pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN)
	{
		theApp.onKeyDown(static_cast<UINT>(pMsg->wParam));
		return TRUE;
	}

	if (pMsg->message == WM_KEYUP)
	{
		theApp.onKeyUp(static_cast<UINT>(pMsg->wParam));
		return TRUE;
	}

	return CFrameWnd::PreTranslateMessage(pMsg);
}