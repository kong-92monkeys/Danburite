// CSceneMenuView01.cpp : implementation file
//

#include "pch.h"
#include "App.h"
#include "CSceneMenuView01.h"


// CSceneMenuView01

IMPLEMENT_DYNCREATE(CSceneMenuView01, CFormView)

CSceneMenuView01::CSceneMenuView01()
	: CFormView(IDD_SCENE_MENU_01)
{

}

CSceneMenuView01::~CSceneMenuView01()
{
}

void CSceneMenuView01::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSceneMenuView01, CFormView)
END_MESSAGE_MAP()


// CSceneMenuView01 diagnostics

#ifdef _DEBUG
void CSceneMenuView01::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CSceneMenuView01::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CSceneMenuView01 message handlers

void CSceneMenuView01::setFPSText(
	double const fps)
{
	CString str;
	str.Format(_T("%.2f"), fps);

	SetDlgItemText(IDC_TEXT_FPS, str);
}