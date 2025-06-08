#pragma once
#include <afxwin.h>
#include <afxcmn.h>
#include <vector>
#include<string>
#include <map>

class Dashboard : public CDialog {
    DECLARE_DYNAMIC(Dashboard)
public:
    Dashboard(const CString& username, bool isAdmin, CWnd* pParent = nullptr);
    void SendCommandToClient(int commandType);

protected:
    virtual BOOL OnInitDialog();
    virtual void DoDataExchange(CDataExchange* pDX);

    CListCtrl m_processList;
    CStatic m_screenView;
    CEdit m_logEdit;
    bool m_isAdmin;

    std::map<SOCKET, CString> m_clients;

    afx_msg void OnShutdown();
    afx_msg void OnReboot();
    afx_msg void OnLock();
    afx_msg void OnTimer(UINT_PTR nIDEvent);

#define WM_UPDATE_PROCESS (WM_USER + 1)
#define WM_UPDATE_SCREEN  (WM_USER + 2)
    afx_msg LRESULT OnUpdateProcess(WPARAM, LPARAM);
    afx_msg LRESULT OnUpdateScreen(WPARAM, LPARAM);

    DECLARE_MESSAGE_MAP()
private:
    SOCKET m_currentClientSocket = INVALID_SOCKET;
    void UpdateProcessList(const std::string& data);
    void UpdateScreenView(const std::vector<BYTE>& imageData);
};
