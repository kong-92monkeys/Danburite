#pragma once



// CSceneMenuView01 form view

class CSceneMenuView01 : public CFormView
{
	DECLARE_DYNCREATE(CSceneMenuView01)

protected:
	CSceneMenuView01();           // protected constructor used by dynamic creation
	virtual ~CSceneMenuView01();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SCENE_MENU_01 };
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


