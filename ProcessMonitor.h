#pragma once
#include <vector>
#include <string>
#include <Windows.h>
#include <tlhelp32.h>

struct ProcessInfo {
    DWORD pid;
    std::string name;
    DWORD memoryUsage;
};

class ProcessMonitor {
public:
    std::vector<ProcessInfo> GetProcessList();
};