
// MainView.cpp : implementation of the CMainView class
//

#include "pch.h"
#include "framework.h"
#include "App.h"
#include "MainView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainView

IMPLEMENT_DYNCREATE(CMainView, CWnd)

CMainView::CMainView()
{
}

CMainView::~CMainView()
{
}


BEGIN_MESSAGE_MAP(CMainView, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CMainView message handlers

BOOL CMainView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(nullptr, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), nullptr);

	return TRUE;
}

void CMainView::OnPaint() 
{
	// TODO: Add your message handler code here
	theApp.render(*__pRenderTarget);
	ValidateRect(nullptr);
}

int CMainView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	__pRenderTarget = theApp.createRenderTarget(GetSafeHwnd());

	__pRenderTargetNeedRedrawListener =
		Infra::EventListener<Render::RenderTarget const *>::bind(
			&CMainView::__onRenderTargetRedrawNeeded, this);

	__pRenderTarget->setBackgroundColor(glm::vec4{ 0.01f, 0.015f, 0.015f, 1.0f });

	return 0;
}

void CMainView::OnDestroy()
{
	CWnd::OnDestroy();

	// TODO: Add your message handler code here
	__pRenderTarget = nullptr;
}

void CMainView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	__pRenderTarget->sync();
}

BOOL CMainView::OnEraseBkgnd(CDC *pDC)
{
	// TODO: Add your message handler code here and/or call default
	// To prevent from erasing window
	return TRUE;
}

void CMainView::__onRenderTargetRedrawNeeded() noexcept
{
	Invalidate(FALSE);
}