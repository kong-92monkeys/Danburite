// CLogView.cpp : implementation file
//

#include "pch.h"
#include "App.h"
#include "LogView.h"


// CLogView

IMPLEMENT_DYNCREATE(CLogView, CFormView)

CLogView::CLogView()
	: CFormView(IDD_LOGVIEW)
{
	__pLoggerImpl = std::make_shared<ListBoxLoggerImpl>(__listLog);
	__pAppIdleListener = Infra::EventListener<>::bind(&CLogView::__onIdle, this);
}

CLogView::~CLogView()
{
}

void CLogView::emplaceLoggerImpl() noexcept
{
	Infra::Logger::emplaceImpl(__pLoggerImpl);
}

void CLogView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LOG, __listLog);
	DDX_Control(pDX, IDC_EDIT_LOG_MSG, __editLogMsg);
}

BEGIN_MESSAGE_MAP(CLogView, CFormView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_CLEAR_LOGS, &CLogView::OnBnClickedClearLogs)
	ON_LBN_SELCHANGE(IDC_LIST_LOG, &CLogView::OnLbnSelchangeListLog)
END_MESSAGE_MAP()

// CLogView diagnostics

#ifdef _DEBUG
void CLogView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CLogView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

void CLogView::__onIdle()
{
	__pLoggerImpl->flush();
}

// CLogView message handlers

int CLogView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	theApp.getIdleEvent() += __pAppIdleListener;

	return 0;
}


void CLogView::OnDestroy()
{
	CFormView::OnDestroy();

	// TODO: Add your message handler code here
	theApp.getIdleEvent() -= __pAppIdleListener;
}

void CLogView::OnBnClickedClearLogs()
{
	__listLog.ResetContent();
	__listLog.SetWindowText(TEXT(""));
}

void CLogView::OnLbnSelchangeListLog()
{
	CString text;
	__listLog.GetText(__listLog.GetCurSel(), text);
	__editLogMsg.SetWindowText(text);
}