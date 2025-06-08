#include "TCPClient.h"
#include <iostream>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

TCPClient::TCPClient(const std::string& serverIP, int port)
    : serverIP(serverIP), port(port) {}

TCPClient::~TCPClient() { Disconnect(); }

bool TCPClient::InitializeWinSock() {
    WSADATA wsaData;
    return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
}

bool TCPClient::Connect() {
    if (!InitializeWinSock()) {
        std::cerr << "WSA 初始化失败，错误码: " << WSAGetLastError() << std::endl;
        return false;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "socket 创建失败，错误码: " << WSAGetLastError() << std::endl;
        return false;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr);

    int result = connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if (result == SOCKET_ERROR) {
        int err = WSAGetLastError();  // 立刻记录
        std::cerr << "连接失败，错误码: " << err << std::endl;

        closesocket(sock);
        sock = INVALID_SOCKET;
        return false;
    }

    std::cout << "成功连接到服务端: " << serverIP << ":" << port << std::endl;
    return true;
}


void TCPClient::Disconnect() {
    if (sock != INVALID_SOCKET) {
        closesocket(sock);
        sock = INVALID_SOCKET;
    }
    WSACleanup();
}

bool TCPClient::SendData(const char* buffer, size_t length) {
    if (sock == INVALID_SOCKET) return false;

    int sent = send(sock, buffer, length, 0);
    return sent == static_cast<int>(length);
}