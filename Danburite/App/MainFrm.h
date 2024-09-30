
// MainFrm.h : interface of the CMainFrame class
//

#pragma once

class CMainFrame : public CFrameWnd
{
	
public:
	CMainFrame() noexcept;
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// Attributes
public:

// Operations
public:
	template <typename $T>
	void replaceSceneMenuView();

	template <typename $T>
	$T *getSceneMenuView();

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext *pContext);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

private:
	CSplitterWnd __windowSplitter;
public:
	virtual BOOL PreTranslateMessage(MSG *pMsg);
};

template <typename $T>
void CMainFrame::replaceSceneMenuView()
{
	int cxCur, cxMin;
	__windowSplitter.GetColumnInfo(1, cxCur, cxMin);

	if (!(__windowSplitter.GetPane(0, 1)->DestroyWindow()))
	{
		TRACE("Cannot destroy the previous view.");
	}

	CCreateContext context{ };
	if (!(__windowSplitter.CreateView(0, 1, RUNTIME_CLASS($T), CSize{ cxCur, 0 }, &context)))
	{
		TRACE("Cannot create the menu view.");
	}

	__windowSplitter.RecalcLayout();
}

template <typename $T>
$T *CMainFrame::getSceneMenuView()
{
	return STATIC_DOWNCAST($T, __windowSplitter.GetPane(0, 1));
}