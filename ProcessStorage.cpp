// Server/ProcessStorage.cpp
#include "pch.h"
#include "ProcessStorage.h"
#include <filesystem>
#include <fstream>
#include <ctime>

namespace fs = std::filesystem;

bool ProcessStorage::SaveProcess(const std::string& clientIP, const std::string& processText) {
    // ��ȡ��ǰʱ��
    time_t now = time(nullptr);
    /*tm* localTime = localtime(&now);*/
    tm localTime; // ��ȷ����tm����
    if (localtime_s(&localTime, &now) != 0) {
        // �������
        return false;
    }

    // ����Ŀ¼��Data/yyyy-MM-dd/IP/process/
    char dateDir[20];
    strftime(dateDir, sizeof(dateDir), "%Y-%m-%d", &localTime);

    fs::path savePath = fs::path("Data") / dateDir / clientIP / "process";
    fs::create_directories(savePath);

    // �����ļ�����HH-MM-SS.txt
    char filename[30];
    strftime(filename, sizeof(filename), "%H-%M-%S.txt", &localTime);

    // д�������Ϣ
    std::ofstream file(savePath / filename, std::ios::out | std::ios::trunc);
    file << processText;
    return file.good();
}
