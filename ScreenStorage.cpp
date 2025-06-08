#include "pch.h"
#include "ScreenStorage.h"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

bool ScreenStorage::SaveScreen(const std::string& clientIP,
    const void* imageData, size_t size) {
    // 创建日期目录
    time_t now = time(nullptr);
    //tm* localTime = localtime(&now);
    tm localTime; // 正确创建tm对象
    if (localtime_s(&localTime, &now) != 0) {
        // 处理错误
        return false;
    }

    char dateDir[20];
    strftime(dateDir, sizeof(dateDir), "%Y-%m-%d", &localTime);

    fs::path savePath = fs::path("Data") / dateDir / clientIP / "screenshots";
    fs::create_directories(savePath);

    // 生成文件名
    char filename[30];
    strftime(filename, sizeof(filename), "%H-%M-%S.jpg", &localTime);

    // 保存文件
    std::ofstream file(savePath / filename, std::ios::binary);
    file.write(static_cast<const char*>(imageData), size);
    return file.good();
}