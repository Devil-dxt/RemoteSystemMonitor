#pragma once
#include <string>
#include <ctime>

class ScreenStorage {
public:
    static bool SaveScreen(const std::string& clientIP,
        const void* imageData, size_t size);
};
