#pragma once
#include <winsock2.h>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <functional>

class TCPServer {
public:
    TCPServer(int port);
    ~TCPServer();

    bool Start();
    void Stop();
    void SendCommand(SOCKET clientSocket, DWORD commandType);
    //std::function<void(const std::string& ip, const std::string& processData)> onProcessDataReceived;
    //std::function<void(const std::string& ip, const std::vector<BYTE>& screenData)> onScreenDataReceived;
    // TCPServer.h
    std::function<void(SOCKET, const std::vector<BYTE>&)> onScreenDataReceived;
    std::function<void(SOCKET, const std::string&)> onProcessDataReceived;

private:
    SOCKET listenSocket = INVALID_SOCKET;
    std::atomic_bool running{ false };
    std::thread listenerThread;
    std::unordered_map<SOCKET, std::thread> clientThreads;
    int port;

    void ListenerLoop();
    void HandleClient(SOCKET clientSocket);
    static void SaveProcessData(const std::string& clientIP, const std::string& data);
    
};
