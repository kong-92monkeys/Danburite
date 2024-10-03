// CSceneMenuView01.cpp : implementation file
//

#include "pch.h"
#include "App.h"
#include "CSceneMenuView01.h"
#include "../Infra/Logger.h"


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
	DDX_Control(pDX, IDC_EDIT_FPS, __editFPS);
	DDX_Control(pDX, IDC_EDIT_LIGHT_COUNT, __editLightCount);
}

BEGIN_MESSAGE_MAP(CSceneMenuView01, CFormView)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LIGHT_COUNT, &CSceneMenuView01::OnDeltaposSpinLightCount)
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

	__editFPS.SetWindowText(str);
	__editFPS.UpdateData(FALSE);
}

void CSceneMenuView01::OnDeltaposSpinLightCount(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	// TODO: Add your control notification handler code here

	int curLightCount{ __lightCount };

	if (pNMUpDown->iDelta < 0)
		++curLightCount;
	else
		--curLightCount;

	curLightCount = std::clamp(curLightCount, 0, __MAX_LIGHT_COUNT);
	if (__lightCount == curLightCount)
		return;

	if (__lightCount < curLightCount)
		__addLightEvent.invoke();
	else
		__removeLightEvent.invoke();
		
	CString str;
	str.Format(_T("%d"), curLightCount);
	__editLightCount.SetWindowText(str);

	__lightCount = curLightCount;

	*pResult = 0;
}

void CSceneMenuView01::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	__editLightCount.SetWindowText(_T("0"));
}
