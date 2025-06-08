// Server/ProcessStorage.cpp
#include "pch.h"
#include "ProcessStorage.h"
#include <filesystem>
#include <fstream>
#include <ctime>

namespace fs = std::filesystem;

bool ProcessStorage::SaveProcess(const std::string& clientIP, const std::string& processText) {
    // 获取当前时间
    time_t now = time(nullptr);
    /*tm* localTime = localtime(&now);*/
    tm localTime; // 正确创建tm对象
    if (localtime_s(&localTime, &now) != 0) {
        // 处理错误
        return false;
    }

    // 构造目录：Data/yyyy-MM-dd/IP/process/
    char dateDir[20];
    strftime(dateDir, sizeof(dateDir), "%Y-%m-%d", &localTime);

    fs::path savePath = fs::path("Data") / dateDir / clientIP / "process";
    fs::create_directories(savePath);

    // 构造文件名：HH-MM-SS.txt
    char filename[30];
    strftime(filename, sizeof(filename), "%H-%M-%S.txt", &localTime);

    // 写入进程信息
    std::ofstream file(savePath / filename, std::ios::out | std::ios::trunc);
    file << processText;
    return file.good();
}
