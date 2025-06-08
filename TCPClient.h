#pragma once
#include <string>
#include <winsock2.h>

class TCPClient {
public:
    TCPClient(const std::string& serverIP, int port);
    ~TCPClient();

    bool Connect();
    void Disconnect();
    bool SendData(const char* buffer, size_t length);
    SOCKET GetSocket() const { return sock; }

private:
    SOCKET sock = INVALID_SOCKET;
    std::string serverIP;
    int port;
    bool InitializeWinSock();
};