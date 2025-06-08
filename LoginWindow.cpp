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
    // �����������ʽ
    m_editPassword.SetPasswordChar('*');
    return TRUE;
}

void LoginWindow::OnBnClickedLogin() {
    UpdateData(TRUE); // ��ȡ��������

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
            MessageBox(_T("�û������������"), _T("��¼ʧ��"), MB_ICONERROR);
        }
    }
    catch (const std::exception& e) {
        MessageBox(_T("���ݿ�����ʧ��"), _T("ϵͳ����"), MB_ICONERROR);
    }
}