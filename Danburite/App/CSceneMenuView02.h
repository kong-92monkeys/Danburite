#pragma once



// CSceneMenuView02 form view

class CSceneMenuView02 : public CFormView
{
	DECLARE_DYNCREATE(CSceneMenuView02)

protected:
	CSceneMenuView02();           // protected constructor used by dynamic creation
	virtual ~CSceneMenuView02();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SCENE_MENU_02 };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	void setFPSText(
		double fps);

private:
	CEdit __editFPS;
};