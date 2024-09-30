
// App.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "App.h"
#include "MainFrm.h"
#include "AboutDlg.h"
#include "../System/Env.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "../Infra/Logger.h"

// CApp

BEGIN_MESSAGE_MAP(CApp, CWinApp)
	ON_COMMAND(ID_SCENES_00, &CApp::OnScenes00)
	ON_COMMAND(ID_SCENES_01, &CApp::OnScenes01)
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
	CMainFrame* pFrame = new CMainFrame;
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
	__pSceneLoader = nullptr;
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

void CApp::setMainView(
	CMainView *const pMainView)
{
	if (__pMainView == pMainView)
		return;

	if (__pMainView)
		__pMainView->enableDisplay(false);

	__pMainView = pMainView;

	if (__pMainView)
		__pMainView->enableDisplay(__pSceneLoader->getSceneType() != SceneType::NOTHING);

	__pSceneLoader->setDisplay(__pMainView ? __pMainView->getDisplay() : nullptr);
}

void CApp::onKeyDown(
	UINT const nChar)
{
	__pSceneLoader->onKeyDown(nChar);
}

void CApp::onKeyUp(
	UINT const nChar)
{
	__pSceneLoader->onKeyUp(nChar);
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
	contextCreateInfo.debugMode = true;
	Infra::Logger::log(Infra::Logger::Severity::INFO, "Debug mode is activated.");
#endif

	__pVulkanContext = std::make_unique<Dev::Context>(contextCreateInfo);
	__pRenderSystem = std::make_unique<Frx::RenderSystem>(
		*__pVulkanContext, __pVulkanContext->getPhysicalDeviceOf(0ULL));

	__pSceneLoader = std::make_unique<SceneLoader>(*__pRenderSystem);
	Infra::Logger::log(Infra::Logger::Severity::INFO, "Init completed successfully.");
}

// CApp message handlers
void CApp::OnScenes00()
{
	// TODO: Add your command handler code here
	if (__pMainView)
		__pMainView->enableDisplay(false);

	__loadScene(SceneType::NOTHING);
}

void CApp::OnScenes01()
{
	// TODO: Add your command handler code here
	if (__pMainView)
		__pMainView->enableDisplay(true);

	__loadScene(SceneType::PHONG_TEST);
}

// App command to run the dialog
void CApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void CApp::__loadScene(
	SceneType const sceneType)
{
	if (__pSceneLoader->getSceneType() == sceneType)
		return;

	__pSceneLoader->load(sceneType);

	Infra::Logger::log(
		Infra::Logger::Severity::INFO,
		std::format("Scene {} is loading now.", static_cast<int>(sceneType)));
}