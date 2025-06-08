#pragma once
#include <string>

class ProcessStorage {
public:
    static bool SaveProcess(const std::string& clientIP, const std::string& processText);
};