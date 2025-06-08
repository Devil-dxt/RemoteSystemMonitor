#include "SystemCommand.h"
#include <wtsapi32.h>
#pragma comment(lib, "wtsapi32.lib")

bool EnableShutdownPrivilege() {
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        return false;
    }

    if (!LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid)) {
        CloseHandle(hToken);
        return false;
    }

    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    BOOL result = AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, NULL, NULL);
    CloseHandle(hToken);

    return (result && GetLastError() == ERROR_SUCCESS);
}

bool SystemCommand::ExecuteCommand(DWORD commandType) {
    switch (commandType) {
    case 1: // ¹Ø»ú
        if (!EnableShutdownPrivilege()) return false;
        return ExitWindowsEx(EWX_POWEROFF | EWX_FORCE, SHTDN_REASON_MAJOR_OTHER);
    case 2: // ÖØÆô
        if (!EnableShutdownPrivilege()) return false;
        return ExitWindowsEx(EWX_REBOOT | EWX_FORCE, SHTDN_REASON_MAJOR_OTHER);
    case 3: // ËøÆÁ
        return LockWorkStation();
    default:
        return false;
    }
}