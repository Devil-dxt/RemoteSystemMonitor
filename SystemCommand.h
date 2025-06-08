#pragma once
#include <Windows.h>

class SystemCommand {
public:
    static bool ExecuteCommand(DWORD commandType);
};