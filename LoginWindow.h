// Server/src/UI/LoginWindow.h
#pragma once
#include <afxwin.h>
#include "AuthManager.h"
#include "Resource.h"

class LoginWindow : public CDialog {
    DECLARE_DYNAMIC(LoginWindow)
public:
    LoginWindow(CWnd* pParent = nullptr);
    virtual ~LoginWindow();

    bool IsAdmin() const { return m_isAdmin; }
    CString GetUsername() const { return m_username; }

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();

    CEdit m_editUsername;
    CEdit m_editPassword;
    CString m_username;
    CString m_password;
    bool m_isAdmin;

    afx_msg void OnBnClickedLogin();

    DECLARE_MESSAGE_MAP()
};
