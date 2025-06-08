
// Server.cpp: 定义应用程序的类行为。
//

#include "pch.h"
#include "framework.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "Server.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "ServerDoc.h"
#include "ServerView.h"

//myself
#include "LoginWindow.h"
#include "DatabaseInit.h"

#include "LoginWindow.h"
#include "Dashboard.h"
#include "ServerConfig.h"

#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")  // 链接GDI+库
using namespace Gdiplus;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CServerApp

BEGIN_MESSAGE_MAP(CServerApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CServerApp::OnAppAbout)
	// 基于文件的标准文档命令
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// 标准打印设置命令
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()


// CServerApp 构造

CServerApp::CServerApp() noexcept
{
	m_bHiColorIcons = TRUE;


	m_nAppLook = 0;
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// 如果应用程序是利用公共语言运行时支持(/clr)构建的，则: 
	//     1) 必须有此附加设置，“重新启动管理器”支持才能正常工作。
	//     2) 在您的项目中，您必须按照生成顺序向 System.Windows.Forms 添加引用。
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: 将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("Server.AppID.NoVersion"));

	// TODO:  在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的 CServerApp 对象

CServerApp theApp;


// CServerApp 初始化

//BOOL CServerApp::InitInstance()
//{
//    // 初始化公共控件
//    INITCOMMONCONTROLSEX InitCtrls;
//    InitCtrls.dwSize = sizeof(InitCtrls);
//    InitCtrls.dwICC = ICC_WIN95_CLASSES;
//    InitCommonControlsEx(&InitCtrls);
//
//    CWinAppEx::InitInstance();
//
//    // 初始化 OLE 库
//    if (!AfxOleInit())
//    {
//        AfxMessageBox(IDP_OLE_INIT_FAILED);
//        return FALSE;
//    }
//
//    AfxEnableControlContainer();
//    EnableTaskbarInteraction();
//
//    // 标准初始化
//    SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
//    LoadStdProfileSettings(4);
//
//    InitContextMenuManager();
//    InitKeyboardManager();
//    InitTooltipManager();
//    CMFCToolTipInfo ttParams;
//    ttParams.m_bVislManagerTheme = TRUE;
//    theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
//        RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);
//
//    // ======== 👇 添加数据库初始化和登录窗口逻辑 👇 ========
//    /*if (!InitializeDatabase())
//    {
//        AfxMessageBox(_T("数据库初始化失败！"));
//        return FALSE;
//    }*/
//
//    LoginWindow loginDlg;
//    if (loginDlg.DoModal() != IDOK)
//    {
//        return FALSE; // 用户点击取消或登录失败，退出应用
//    }
//
//    // ======== 👆 登录成功后继续原来的初始化 👆 ========
//
//    // 注册文档模板
//    CMultiDocTemplate* pDocTemplate;
//    pDocTemplate = new CMultiDocTemplate(IDR_ServerTYPE,
//        RUNTIME_CLASS(CServerDoc),
//        RUNTIME_CLASS(CChildFrame),
//        RUNTIME_CLASS(CServerView));
//    if (!pDocTemplate)
//        return FALSE;
//    AddDocTemplate(pDocTemplate);
//
//    // 创建主 MDI 框架窗口
//    CMainFrame* pMainFrame = new CMainFrame;
//    if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
//    {
//        delete pMainFrame;
//        return FALSE;
//    }
//    m_pMainWnd = pMainFrame;
//
//    // 处理命令行参数
//    CCommandLineInfo cmdInfo;
//    ParseCommandLine(cmdInfo);
//
//    if (!ProcessShellCommand(cmdInfo))
//        return FALSE;
//
//    // 显示主窗口
//    pMainFrame->ShowWindow(m_nCmdShow);
//    pMainFrame->UpdateWindow();
//
//    return TRUE;
//}

BOOL CServerApp::InitInstance()
{
    // 初始化公共控件
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinAppEx::InitInstance();

    // 初始化 OLE
    if (!AfxOleInit())
    {
        AfxMessageBox(IDP_OLE_INIT_FAILED);
        return FALSE;
    }

    AfxEnableControlContainer();
    EnableTaskbarInteraction();

    // 注册表 & 配置
    SetRegistryKey(_T("RemoteMonitorSystem"));
    LoadStdProfileSettings(4);

    InitContextMenuManager();
    InitKeyboardManager();
    InitTooltipManager();
    CMFCToolTipInfo ttParams;
    ttParams.m_bVislManagerTheme = TRUE;
    theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
        RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

    // 初始化 GDI+
    GdiplusStartupInput gdiplusStartupInput;
    static ULONG_PTR gdiplusToken = 0;  // 注意保持作用域
    if (GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL) != Ok)
    {
        AfxMessageBox(_T("GDI+ 初始化失败"));
        return FALSE;
    }

    // 加载服务端配置
    ServerConfig config = ServerConfigLoader::LoadConfig("server.ini");
    // TODO: 将 config 中内容应用到系统设置中

    // 显示登录窗口
    LoginWindow loginDlg;
    if (loginDlg.DoModal() != IDOK)
    {
        return FALSE; // 登录失败或取消退出
    }

    Dashboard dashboard(loginDlg.GetUsername(), loginDlg.IsAdmin());
    m_pMainWnd = &dashboard;
    dashboard.DoModal();

    return FALSE;  // 关闭应用
}



int CServerApp::ExitInstance()
{
	//TODO: 处理可能已添加的附加资源
	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}

// CServerApp 消息处理程序


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg() noexcept;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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

// 用于运行对话框的应用程序命令
void CServerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CServerApp 自定义加载/保存方法

void CServerApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

void CServerApp::LoadCustomState()
{
}

void CServerApp::SaveCustomState()
{
}

// CServerApp 消息处理程序



