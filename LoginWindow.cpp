// Server/src/UI/LoginWindow.cpp
#include "pch.h"
#include "LoginWindow.h"

IMPLEMENT_DYNAMIC(LoginWindow, CDialog)

BEGIN_MESSAGE_MAP(LoginWindow, CDialog)
    ON_BN_CLICKED(IDOK, &LoginWindow::OnBnClickedLogin)
END_MESSAGE_MAP()

LoginWindow::LoginWindow(CWnd* pParent)
    : CDialog(IDD_LOGIN_DIALOG, pParent), m_isAdmin(false) {}

LoginWindow::~LoginWindow() {}

void LoginWindow::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_USERNAME, m_editUsername);
    DDX_Control(pDX, IDC_EDIT_PASSWORD, m_editPassword);
    DDX_Text(pDX, IDC_EDIT_USERNAME, m_username);
    DDX_Text(pDX, IDC_EDIT_PASSWORD, m_password);
}

BOOL LoginWindow::OnInitDialog() {
    CDialog::OnInitDialog();
    // 设置密码框样式
    m_editPassword.SetPasswordChar('*');
    return TRUE;
}

void LoginWindow::OnBnClickedLogin() {
    UpdateData(TRUE); // 获取输入数据

    try {
        AuthManager auth;
        bool isAdmin = false;
        if (auth.Authenticate(
            std::string(CW2A(m_username.GetString())),
            std::string(CW2A(m_password.GetString())),
            isAdmin)) {
            m_isAdmin = isAdmin;
            EndDialog(IDOK);
        }
        else {
            MessageBox(_T("用户名或密码错误"), _T("登录失败"), MB_ICONERROR);
        }
    }
    catch (const std::exception& e) {
        MessageBox(_T("数据库连接失败"), _T("系统错误"), MB_ICONERROR);
    }
}