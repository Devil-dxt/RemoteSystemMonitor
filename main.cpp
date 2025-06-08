// Client/src/main.cpp
#include <iostream>
#include <sstream>  
#include <thread>
#include <atomic>
#include <winsock2.h>     // 包含 Winsock API
#include <algorithm>      // 包含 std::min
#pragma comment(lib, "ws2_32.lib")  // 链接 Winsock 库
#include "TCPClient.h"
#include "ScreenCapture.h"
#include "ProcessMonitor.h"
#include "ClientConfig.h"
#include "MessageDef.h"
#include "SystemCommand.h"

std::atomic_bool running(true);

void SendScreenData(TCPClient& client, ScreenCapturer& capturer, int quality) {
    while (running) {
        auto imageData = capturer.CaptureScreen(quality);

        MessageHeader header{
            .msgType = MSG_SCREEN_DATA,
            .msgStatus = 0,
            .dataLength = static_cast<uint32_t>(imageData.size())
        };

        std::vector<char> buffer(sizeof(header) + imageData.size());
        memcpy(buffer.data(), &header, sizeof(header));
        memcpy(buffer.data() + sizeof(header), imageData.data(), imageData.size());

        if (!client.SendData(buffer.data(), buffer.size())) {
            std::cerr << "Failed to send screen data" << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

void SendProcessData(TCPClient& client, ProcessMonitor& monitor) {
    while (running) {
        auto processes = monitor.GetProcessList();

        std::stringstream ss;
        for (const auto& p : processes) {
            ss << p.pid << "," << p.name << "," << p.memoryUsage << "\n";
        }

        std::string data = ss.str();
        MessageHeader header{
            .msgType = MSG_PROCESS_DATA,
            .msgStatus = 0,
            .dataLength = static_cast<uint32_t>(data.size())
        };

        std::vector<char> buffer(sizeof(header) + data.size());
        memcpy(buffer.data(), &header, sizeof(header));
        memcpy(buffer.data() + sizeof(header), data.data(), data.size());

        if (!client.SendData(buffer.data(), buffer.size())) {
            std::cerr << "Failed to send process data" << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::seconds(60));
    }
}

void ReceiveCommand(TCPClient& client) {
    while (running) {
        MessageHeader header;
        int received = recv(client.GetSocket(), (char*)&header, sizeof(header), 0);
        if (received <= 0) break;

        if (header.msgType == MSG_SYS_COMMAND && header.dataLength == sizeof(int)) {
            int command = 0;
            received = recv(client.GetSocket(), (char*)&command, sizeof(command), 0);
            if (received <= 0) break;
            std::cout << "Received command: " << command << std::endl;
            SystemCommand::ExecuteCommand(command);
        }
        else {
            // 处理其他情况（保留扩展性）
            char dummy[1024];
            uint32_t toRead = header.dataLength < sizeof(dummy) ? header.dataLength : sizeof(dummy);
            recv(client.GetSocket(), dummy, toRead, 0);
        }
    }
}

int main() {
    ClientConfig config = ConfigLoader::LoadConfig("client.ini");
    std::cout << "服务器地址: " << config.serverIP << "  端口: " << config.port << std::endl;

    //手动确认服务端登录完成
    std::cout << "请确认服务端已启动并完成登录后按下回车键继续..." << std::endl;
    std::cin.get();
    TCPClient client(config.serverIP, config.port);

    if (!client.Connect()) {
        std::cerr << "Failed to connect to server" << WSAGetLastError() << std::endl;
        return 1;
    }

    ScreenCapturer screenCapturer;
    ProcessMonitor processMonitor;

    std::thread screenThread(SendScreenData,
        std::ref(client),
        std::ref(screenCapturer),
        config.jpegQuality);

    std::thread processThread(SendProcessData,
        std::ref(client),
        std::ref(processMonitor));

    std::thread commandThread(ReceiveCommand, std::ref(client));

    // 等待退出信号
    std::cout << "Client running. Press Enter to exit..." << std::endl;
    std::cin.get();
    running = false;

    screenThread.join();
    processThread.join();
    commandThread.join();
    return 0;
}