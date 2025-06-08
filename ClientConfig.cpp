#include "ClientConfig.h"
#include <fstream>
#include <sstream>

ClientConfig ConfigLoader::LoadConfig(const std::string& path) {
    ClientConfig config;
    std::ifstream file(path);

    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find('=');
        if (pos == std::string::npos) continue;

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        if (key == "ServerIP") config.serverIP = value;
        else if (key == "Port") config.port = std::stoi(value);
        else if (key == "ScreenInterval") config.screenInterval = std::stoi(value);
        else if (key == "ProcessInterval") config.processInterval = std::stoi(value);
        else if (key == "JpegQuality") config.jpegQuality = std::stoi(value);
    }

    return config;
}