#pragma once

#include "../Infra/Event.h"

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

public:
	void setFPSText(
		double fps);

	[[nodiscard]]
	constexpr Infra::EventView<> &getAddLightEvent() const noexcept;

	[[nodiscard]]
	constexpr Infra::EventView<> &getRemoveLightEvent() const noexcept;

private:
	CEdit __editFPS;
	CEdit __editLightCount;

	int __lightCount{ };

	mutable Infra::Event<> __addLightEvent;
	mutable Infra::Event<> __removeLightEvent;

public:
	afx_msg void OnDeltaposSpinLightCount(NMHDR *pNMHDR, LRESULT *pResult);
	virtual void OnInitialUpdate();
};

constexpr Infra::EventView<> &CSceneMenuView01::getAddLightEvent() const noexcept
{
	return __addLightEvent;
}

constexpr Infra::EventView<> &CSceneMenuView01::getRemoveLightEvent() const noexcept
{
	return __removeLightEvent;
}