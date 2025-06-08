#pragma once
#include <string>

struct ClientConfig {
    std::string serverIP = "127.0.0.1";
    int port = 8888;
    int screenInterval = 2;
    int processInterval = 60;
    int jpegQuality = 75;
};

class ConfigLoader {
public:
    static ClientConfig LoadConfig(const std::string& path);
};