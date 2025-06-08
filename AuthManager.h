// Server/src/Auth/AuthManager.h
#pragma once
#include <sqlite3.h>
#include <string>

class AuthManager {
public:
    AuthManager();
    ~AuthManager();

    bool Authenticate(const std::string& username,
        const std::string& password,
        bool& isAdmin);

private:
    sqlite3* db;
    bool OpenDatabase();
};
