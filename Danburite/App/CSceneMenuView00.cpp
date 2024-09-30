// CSceneMenuView00.cpp : implementation file
//

#include "pch.h"
#include "App.h"
#include "CSceneMenuView00.h"


// CSceneMenuView00

IMPLEMENT_DYNCREATE(CSceneMenuView00, CFormView)

CSceneMenuView00::CSceneMenuView00()
	: CFormView(IDD_SCENE_MENU_00)
{

}

CSceneMenuView00::~CSceneMenuView00()
{
}

void CSceneMenuView00::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSceneMenuView00, CFormView)
END_MESSAGE_MAP()


// CSceneMenuView00 diagnostics

#ifdef _DEBUG
void CSceneMenuView00::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CSceneMenuView00::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CSceneMenuView00 message handlers
