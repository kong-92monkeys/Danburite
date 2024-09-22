
// App.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "App.h"
#include "MainFrm.h"
#include "../Infra/Bitmap.h"
#include "../System/Env.h"
#include "../Render/TextureUtil.h"
#include "../Frameworks/VertexAttribute.h"

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
	__pRenderer = nullptr;
	__pMaterial = nullptr;
	__pTexture = nullptr;
	__pDrawParam = nullptr;
	__pMesh = nullptr;
	__pObject = nullptr;
	__pLayer = nullptr;

	__pRenderEngine = nullptr;
	__pVulkanContext = nullptr;

	//TODO: handle additional resources you may have added
	return CWinApp::ExitInstance();
}

std::unique_ptr<Render::RenderTarget> CApp::createRenderTarget(
	HWND const hWindow)
{
	auto pRetVal{ __pRenderEngine->createRenderTarget(m_hInstance, hWindow) };
	__setupRenderTarget(*pRetVal);
	return std::unique_ptr<Render::RenderTarget>{ pRetVal };
}

void CApp::render(Render::RenderTarget &renderTarget)
{
	__pRenderEngine->reserveRender(&renderTarget);
}

BOOL CApp::OnIdle(LONG lCount)
{
	// TODO: Add your specialized code here and/or call the base class
	__idleEvent.invoke();
	__pRenderEngine->render();

	return CWinApp::OnIdle(lCount);
}

// CApp message handlers

void CApp::__onInitBeforeMainFrame()
{
	auto &assetManager{ Sys::Env::getInstance().getAssetManager() };
	assetManager.setRootPath("Assets");

	Dev::Context::CreateInfo contextCreateInfo
	{
		.loaderLibName			{ "vulkan_loader_dedicated-1.dll" },
		.engineName				{ "Danburite" },
		.engineVersion			{ 1U, 0U, 0U, 0U },
		.appName				{ "Danburite Sample App" },
		.appVersion				{ 1U, 0U, 0U, 0U }
	};

#ifndef NDEBUG
	contextCreateInfo.debugMode = true;
#endif

	std::unordered_map<std::type_index, uint32_t> materialTypeIds;

	__pVulkanContext = std::make_unique<Dev::Context>(contextCreateInfo);

	Render::GlobalDescriptorManager::BindingInfo globalDescBindingInfo;
	globalDescBindingInfo.materialBufferLocations[typeid(Frx::ImageMaterial)]	= 1U;

	__pRenderEngine = std::make_unique<Render::Engine>(
		*__pVulkanContext,
		__pVulkanContext->getPhysicalDeviceOf(0ULL),
		globalDescBindingInfo);
}

void CApp::__setupRenderTarget(
	Render::RenderTarget &renderTarget)
{
	__pLayer = std::unique_ptr<Render::Layer>{ __pRenderEngine->createLayer() };
	renderTarget.addLayer(__pLayer.get());

	__pObject = std::make_unique<Render::RenderObject>();
	__pLayer->addRenderObject(__pObject.get());

	Infra::GenericBuffer posBuffer;
	posBuffer.typedAdd<glm::vec3>({ -0.5f, -0.5f, 0.5f });
	posBuffer.typedAdd<glm::vec3>({ -0.5f, 0.5f, 0.5f });
	posBuffer.typedAdd<glm::vec3>({ 0.5f, 0.5f, 0.5f });
	posBuffer.typedAdd<glm::vec3>({ 0.5f, -0.5f, 0.5f });

	Infra::GenericBuffer uvBuffer;
	uvBuffer.typedAdd<glm::vec2>({ 0.0f, 0.0f });
	uvBuffer.typedAdd<glm::vec2>({ 0.0f, 1.0f });
	uvBuffer.typedAdd<glm::vec2>({ 1.0f, 1.0f });
	uvBuffer.typedAdd<glm::vec2>({ 1.0f, 0.0f });

	Infra::GenericBuffer indexBuffer;
	indexBuffer.typedAdd<uint16_t>({ 0U, 1U, 2U, 0U, 2U, 3U });

	__pMesh = std::unique_ptr<Render::Mesh>{ __pRenderEngine->createMesh() };
	__pMesh->createVertexBuffer(Frx::VertexAttrib::POS_LOCATION, posBuffer.getData(), posBuffer.getSize());
	__pMesh->createVertexBuffer(Frx::VertexAttrib::UV_LOCATION, uvBuffer.getData(), uvBuffer.getSize());
	__pMesh->createIndexBuffer(VkIndexType::VK_INDEX_TYPE_UINT16, indexBuffer.getData(), indexBuffer.getSize());

	__pDrawParam = std::make_unique<Render::DrawParamIndexed>(6U, 0U, 0);
	__pRenderer = std::unique_ptr<Frx::ImageRenderer>{ __pRenderEngine->createRenderer<Frx::ImageRenderer>() };

	__pObject->setRenderer(__pRenderer.get());
	__pObject->setDrawParam(__pDrawParam.get());
	__pObject->setMesh(__pMesh.get());

	__pTexture = std::unique_ptr<Render::Texture>
	{
		Render::TextureUtil::loadTexture(
			*__pRenderEngine, "Images/smile.jpg",
			VK_PIPELINE_STAGE_2_NONE, VK_ACCESS_2_NONE,
			VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT, VK_ACCESS_2_SHADER_SAMPLED_READ_BIT)
	};

	__pMaterial = std::unique_ptr<Frx::ImageMaterial>{ __pRenderEngine->createMaterial<Frx::ImageMaterial>() };
	__pObject->getMaterialPackOf(0U).setMaterial(__pMaterial.get());

	__pMaterial->setTexture(__pTexture.get());
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



