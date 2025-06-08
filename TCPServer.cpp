#include "pch.h"
#include "TCPServer.h"
#include <iostream>
#include <ws2tcpip.h>
#include "ScreenStorage.h"
#include "ProcessStorage.h"
#include "MessageDef.h"

#pragma comment(lib, "ws2_32.lib")

TCPServer::TCPServer(int port) : port(port) {}

TCPServer::~TCPServer() { Stop(); }

bool TCPServer::Start() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return false;

    listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == INVALID_SOCKET) return false;

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        closesocket(listenSocket);
        return false;
    }

    listen(listenSocket, SOMAXCONN);
    running = true;
    listenerThread = std::thread(&TCPServer::ListenerLoop, this);
    return true;
}

void TCPServer::ListenerLoop() {
    while (running) {
        SOCKET clientSocket = accept(listenSocket, NULL, NULL);
        if (clientSocket != INVALID_SOCKET) {
            clientThreads.emplace(clientSocket,
                std::thread(&TCPServer::HandleClient, this, clientSocket));
        }
    }
}

void TCPServer::HandleClient(SOCKET clientSocket) {
    char buffer[4096];
    std::string clientIP;

    sockaddr_in clientAddr{};
    int addrLen = sizeof(clientAddr);
    char ipBuffer[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &(clientAddr.sin_addr), ipBuffer, sizeof(ipBuffer)) != nullptr) {
        clientIP = ipBuffer;
    }
    else {
        clientIP = "unknown";
    }

    while (true) {
        MessageHeader header;
        int received = recv(clientSocket, (char*)&header, sizeof(header), 0);
        if (received <= 0) break;

        std::vector<char> data(header.dataLength);
        int totalReceived = 0;
        while (totalReceived < header.dataLength) {
            received = recv(clientSocket, data.data() + totalReceived,
                header.dataLength - totalReceived, 0);
            if (received <= 0) break;
            totalReceived += received;
        }

        switch (header.msgType) {
        case MSG_SCREEN_DATA:
            ScreenStorage::SaveScreen(clientIP, data.data(), data.size());
            if (onScreenDataReceived)
                onScreenDataReceived(clientSocket, std::vector<BYTE>(data.begin(), data.end()));
            break;
        case MSG_PROCESS_DATA:
            ProcessStorage::SaveProcess(clientIP, std::string(data.begin(), data.end()));
            if (onProcessDataReceived)
                onProcessDataReceived(clientSocket, std::string(data.begin(), data.end()));
            break;
        }
    }

    closesocket(clientSocket);
}

void TCPServer::Stop() {
    running = false;
    if (listenSocket != INVALID_SOCKET) {
        closesocket(listenSocket);
        listenSocket = INVALID_SOCKET;
    }
    for (auto& [sock, thread] : clientThreads) {
        if (thread.joinable()) thread.join();
    }
    WSACleanup();
}

void TCPServer::SendCommand(SOCKET clientSocket, DWORD commandType) {
    /*MessageHeader header{
        .msgType = MSG_SYS_COMMAND,
        .msgStatus = 0,
        .dataLength = sizeof(DWORD)
    };*/
    MessageHeader header{
    MSG_SYS_COMMAND,
    0,
    sizeof(DWORD)
    };

    std::vector<char> buffer(sizeof(header) + sizeof(DWORD));
    memcpy(buffer.data(), &header, sizeof(header));
    memcpy(buffer.data() + sizeof(header), &commandType, sizeof(DWORD));

    send(clientSocket, buffer.data(), buffer.size(), 0);
}