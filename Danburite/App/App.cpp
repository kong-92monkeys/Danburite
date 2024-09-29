
// App.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "App.h"
#include "MainFrm.h"
#include "../System/Env.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CApp

BEGIN_MESSAGE_MAP(CApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CApp::OnAppAbout)
END_MESSAGE_MAP()


// CApp construction

CApp::CApp() noexcept
{

	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("App.AppID.NoVersion"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CApp object

CApp theApp;


// CApp initialization

BOOL CApp::InitInstance()
{
	CWinApp::InitInstance();


	EnableTaskbarInteraction(FALSE);

	// AfxInitRichEdit2() is required to use RichEdit control
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));


	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object
	CFrameWnd* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;

	__onInitBeforeMainFrame();

	// create and load the frame with its resources
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, nullptr,
		nullptr);

	// The one and only window has been initialized, so show and update it
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();
	return TRUE;
}

int CApp::ExitInstance()
{
	__pScene = nullptr;
	__pRenderSystem = nullptr;
	__pVulkanContext = nullptr;

	//TODO: handle additional resources you may have added
	return CWinApp::ExitInstance();
}

std::unique_ptr<Frx::Display> CApp::createDisplay(
	HWND const hwnd,
	bool const useDepthBuffer,
	bool const useStencilBuffer)
{
	return __pRenderSystem->createDisplay(
		m_hInstance, hwnd, useDepthBuffer, useStencilBuffer);
}

void CApp::setSceneDisplay(
	Frx::Display *const pDisplay)
{
	__pScene->setDisplay(pDisplay);
}

void CApp::onViewKeyDown(
	UINT const nChar)
{
	switch (nChar)
	{
		case 'd':
		case 'D':
			__pScene->startCameraMoveRight();
			break;

		case 'a':
		case 'A':
			__pScene->startCameraMoveLeft();
			break;

		case 'e':
		case 'E':
			__pScene->startCameraMoveUp();
			break;

		case 'q':
		case 'Q':
			__pScene->startCameraMoveDown();
			break;

		case 'w':
		case 'W':
			__pScene->startCameraMoveForward();
			break;

		case 's':
		case 'S':
			__pScene->startCameraMoveBackward();
			break;

		case VK_RIGHT:
			__pScene->startCameraRotateRight();
			break;

		case VK_LEFT:
			__pScene->startCameraRotateLeft();
			break;

		case VK_UP:
			__pScene->startCameraRotateUp();
			break;

		case VK_DOWN:
			__pScene->startCameraRotateDown();
			break;
	}
}

void CApp::onViewKeyUp(
	UINT const nChar)
{
	switch (nChar)
	{
		case 'd':
		case 'D':
			__pScene->endCameraMoveRight();
			break;

		case 'a':
		case 'A':
			__pScene->endCameraMoveLeft();
			break;

		case 'e':
		case 'E':
			__pScene->endCameraMoveUp();
			break;

		case 'q':
		case 'Q':
			__pScene->endCameraMoveDown();
			break;

		case 'w':
		case 'W':
			__pScene->endCameraMoveForward();
			break;

		case 's':
		case 'S':
			__pScene->endCameraMoveBackward();
			break;

		case VK_RIGHT:
			__pScene->endCameraRotateRight();
			break;

		case VK_LEFT:
			__pScene->endCameraRotateLeft();
			break;

		case VK_UP:
			__pScene->endCameraRotateUp();
			break;

		case VK_DOWN:
			__pScene->endCameraRotateDown();
			break;
	}
}

BOOL CApp::OnIdle(LONG lCount)
{
	// TODO: Add your specialized code here and/or call the base class
	__idleEvent.invoke();
	return CWinApp::OnIdle(lCount);
}

// CApp message handlers

void CApp::__onInitBeforeMainFrame()
{
	auto &assetManager{ Sys::Env::getInstance().getAssetManager() };
	assetManager.setRootPath("Assets");

	Dev::Context::CreateInfo contextCreateInfo
	{
		.loaderLibName		{ "vulkan_loader_dedicated-1.dll" },
		.engineName			{ "Danburite" },
		.engineVersion		{ 1U, 0U, 0U, 0U },
		.appName			{ "Danburite Sample App" },
		.appVersion			{ 1U, 0U, 0U, 0U }
	};

#ifndef NDEBUG
	//contextCreateInfo.debugMode = true;
#endif

	__pVulkanContext = std::make_unique<Dev::Context>(contextCreateInfo);
	__pRenderSystem = std::make_unique<Frx::RenderSystem>(
		*__pVulkanContext, __pVulkanContext->getPhysicalDeviceOf(0ULL));

	__pScene = __pRenderSystem->createScene<PhongTestScene>();
}

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg() noexcept;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() noexcept : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// App command to run the dialog
void CApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CApp message handlers



