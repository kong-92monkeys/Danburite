#pragma once



// CSceneMenuView00 form view

class CSceneMenuView00 : public CFormView
{
	DECLARE_DYNCREATE(CSceneMenuView00)

protected:
	CSceneMenuView00();           // protected constructor used by dynamic creation
	virtual ~CSceneMenuView00();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SCENE_MENU_DEFAULT };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};


