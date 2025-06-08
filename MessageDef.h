#pragma once
#include <cstdint>

enum MsgType : uint32_t {
    MSG_SCREEN_DATA = 1,
    MSG_PROCESS_DATA = 2,
    MSG_SYS_COMMAND = 3
};

enum CommandType : uint32_t {
    CMD_SHUTDOWN = 1,
    CMD_REBOOT = 2,
    CMD_LOCK = 3
};

#pragma pack(push, 1)
struct MessageHeader {
    uint32_t msgType;
    uint32_t msgStatus;
    uint32_t dataLength;
};
#pragma pack(pop)