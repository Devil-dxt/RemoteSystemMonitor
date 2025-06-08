#include "pch.h"
#include "Dashboard.h"
#include "TCPServer.h"
#include "Resource.h"
#include "MessageDef.h"

#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

// Dashboard.cpp 顶部添加：
#include <string>
#include <sstream>
#include <atlconv.h>   // 支持 CA2T 宏转换

using namespace Gdiplus;

IMPLEMENT_DYNAMIC(Dashboard, CDialog)

BEGIN_MESSAGE_MAP(Dashboard, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_SHUTDOWN, &Dashboard::OnShutdown)
    ON_BN_CLICKED(IDC_BUTTON_REBOOT, &Dashboard::OnReboot)
    ON_BN_CLICKED(IDC_BUTTON_LOCK, &Dashboard::OnLock)
    ON_WM_TIMER()
    ON_MESSAGE(WM_UPDATE_PROCESS, &Dashboard::OnUpdateProcess)
    ON_MESSAGE(WM_UPDATE_SCREEN, &Dashboard::OnUpdateScreen)
END_MESSAGE_MAP()


CString UTF8ToCString(const std::string& utf8Str) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, &wstrTo[0], size_needed);
    return CString(wstrTo.c_str());
}

LRESULT Dashboard::OnUpdateProcess(WPARAM wParam, LPARAM lParam) {
    std::string* data = reinterpret_cast<std::string*>(wParam);
    m_currentClientSocket = static_cast<SOCKET>(lParam); // 保存 socket
    UpdateProcessList(*data);
    delete data;
    return 0;
}

LRESULT Dashboard::OnUpdateScreen(WPARAM wParam, LPARAM lParam) {
    std::vector<BYTE>* imageData = reinterpret_cast<std::vector<BYTE>*>(wParam);
    m_currentClientSocket = static_cast<SOCKET>(lParam); // 保存 socket
    UpdateScreenView(*imageData);
    delete imageData;
    return 0;
}

Dashboard::Dashboard(const CString& username, bool isAdmin, CWnd* pParent)
    : CDialog(IDD_DASHBOARD, pParent), m_isAdmin(isAdmin) {}

BOOL Dashboard::OnInitDialog() {
    CDialog::OnInitDialog();

    // 初始化控件
    m_processList.InsertColumn(0, _T("进程名"), LVCFMT_LEFT, 150);
    m_processList.InsertColumn(1, _T("PID"), LVCFMT_LEFT, 80);
    m_processList.InsertColumn(2, _T("内存(KB)"), LVCFMT_LEFT, 100);

    // 启动TCP服务器
    TCPServer* server = new TCPServer(8888);
    server->Start();

    server->onProcessDataReceived = [this](SOCKET clientSocket, const std::string& data) {
        auto* pData = new std::string(data);
        PostMessage(WM_UPDATE_PROCESS, (WPARAM)pData, (LPARAM)clientSocket);
        };

    server->onScreenDataReceived = [this](SOCKET clientSocket, const std::vector<BYTE>& image) {
        auto* pImage = new std::vector<BYTE>(image);
        PostMessage(WM_UPDATE_SCREEN, (WPARAM)pImage, (LPARAM)clientSocket);
        };

    SetTimer(1, 2000, NULL); // 2秒更新定时器
    return TRUE;
}

void Dashboard::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_PROCESS, m_processList);
    DDX_Control(pDX, IDC_STATIC_SCREEN, m_screenView);
    DDX_Control(pDX, IDC_EDIT_LOG, m_logEdit);
}

void Dashboard::OnTimer(UINT_PTR nIDEvent) {
    if (nIDEvent == 1) {
        // 更新界面显示
    }
    CDialog::OnTimer(nIDEvent);
}

void Dashboard::UpdateProcessList(const std::string& data) {
    m_processList.DeleteAllItems();

    std::stringstream ss(data);
    std::string line;
    int index = 0;

    while (std::getline(ss, line)) {
        std::stringstream lineStream(line);
        std::string pid, name, memory;

        std::getline(lineStream, pid, ',');
        std::getline(lineStream, name, ',');
        std::getline(lineStream, memory);

        CString cname = UTF8ToCString(name);
        CString cpid = UTF8ToCString(pid);
        CString cmemory = UTF8ToCString(memory);

        m_processList.InsertItem(index, cname);
        m_processList.SetItemText(index, 1, cpid);
        m_processList.SetItemText(index, 2, cmemory);
        index++;
    }
}

void Dashboard::UpdateScreenView(const std::vector<BYTE>& imageData) {
    IStream* pStream = nullptr;
    if (CreateStreamOnHGlobal(NULL, TRUE, &pStream) == S_OK) {
        ULONG written = 0;
        if (pStream->Write(imageData.data(), imageData.size(), &written) == S_OK && written == imageData.size()) {
            // 回到流开头
            LARGE_INTEGER liZero = {};
            pStream->Seek(liZero, STREAM_SEEK_SET, NULL);

            CImage image;
            if (SUCCEEDED(image.Load(pStream))) {
                CDC* pDC = m_screenView.GetDC();
                CRect rect;
                m_screenView.GetClientRect(&rect);
                image.StretchBlt(pDC->GetSafeHdc(), 0, 0, rect.Width(), rect.Height(), SRCCOPY);
                m_screenView.ReleaseDC(pDC);
            }
            else {
                AfxMessageBox(_T("图像解码失败，请确认图像数据是否为 JPEG/PNG 格式"));
            }
        }
        else {
            AfxMessageBox(_T("写入内存流失败"));
        }
        pStream->Release();
    }
}

void Dashboard::SendCommandToClient(int commandType) {
    if (m_currentClientSocket == INVALID_SOCKET) {
        AfxMessageBox(_T("没有可用的客户端连接"));
        return;
    }
    MessageHeader header;
    header.msgType = MSG_SYS_COMMAND;
    header.dataLength = sizeof(int);

    // 先发送消息头
    send(m_currentClientSocket, (char*)&header, sizeof(header), 0);
    // 再发送命令本体（如 CMD_SHUTDOWN）
    send(m_currentClientSocket, (char*)&commandType, sizeof(int), 0);
}

void Dashboard::OnShutdown() {
    SendCommandToClient(CMD_SHUTDOWN);
}

void Dashboard::OnReboot() {
    SendCommandToClient(CMD_REBOOT);
}

void Dashboard::OnLock() {
    SendCommandToClient(CMD_LOCK);
}

