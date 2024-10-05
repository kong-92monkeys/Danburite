// CSceneMenuView02.cpp : implementation file
//

#include "pch.h"
#include "App.h"
#include "CSceneMenuView02.h"


// CSceneMenuView02

IMPLEMENT_DYNCREATE(CSceneMenuView02, CFormView)

CSceneMenuView02::CSceneMenuView02()
	: CFormView(IDD_SCENE_MENU_02)
{

}

CSceneMenuView02::~CSceneMenuView02()
{
}

void CSceneMenuView02::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_FPS, __editFPS);
}

BEGIN_MESSAGE_MAP(CSceneMenuView02, CFormView)
END_MESSAGE_MAP()


// CSceneMenuView02 diagnostics

#ifdef _DEBUG
void CSceneMenuView02::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CSceneMenuView02::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CSceneMenuView02 message handlers

void CSceneMenuView02::setFPSText(
	double const fps)
{
	CString str;
	str.Format(_T("%.2f"), fps);

	__editFPS.SetWindowText(str);
	__editFPS.UpdateData(FALSE);
}