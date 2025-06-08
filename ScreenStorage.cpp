#include "pch.h"
#include "ScreenStorage.h"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

bool ScreenStorage::SaveScreen(const std::string& clientIP,
    const void* imageData, size_t size) {
    // ��������Ŀ¼
    time_t now = time(nullptr);
    //tm* localTime = localtime(&now);
    tm localTime; // ��ȷ����tm����
    if (localtime_s(&localTime, &now) != 0) {
        // �������
        return false;
    }

    char dateDir[20];
    strftime(dateDir, sizeof(dateDir), "%Y-%m-%d", &localTime);

    fs::path savePath = fs::path("Data") / dateDir / clientIP / "screenshots";
    fs::create_directories(savePath);

    // �����ļ���
    char filename[30];
    strftime(filename, sizeof(filename), "%H-%M-%S.jpg", &localTime);

    // �����ļ�
    std::ofstream file(savePath / filename, std::ios::binary);
    file.write(static_cast<const char*>(imageData), size);
    return file.good();
}