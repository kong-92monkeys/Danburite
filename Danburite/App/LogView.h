#pragma once

#include "ListBoxLoggerImpl.h"
#include "../Infra/Event.h"

// CLogView form view

class CLogView : public CFormView
{
	DECLARE_DYNCREATE(CLogView)

protected:
	CLogView();           // protected constructor used by dynamic creation
	virtual ~CLogView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LOGVIEW };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedClearLogs();
	afx_msg void OnLbnSelchangeListLog();

public:
	void emplaceLoggerImpl() noexcept;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	CListBox __listLog;
	CEdit __editLogMsg;

	std::shared_ptr<ListBoxLoggerImpl> __pLoggerImpl;
	Infra::EventListenerPtr<> __pAppIdleListener;

	void __onIdle();
};


