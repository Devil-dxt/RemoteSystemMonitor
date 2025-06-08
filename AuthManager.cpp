// Server/src/Auth/AuthManager.cpp
#include "pch.h"
#include "AuthManager.h"
#include <iostream>

AuthManager::AuthManager() : db(nullptr) {
    if (!OpenDatabase()) {
        throw std::runtime_error("无法连接数据库");
    }
}

AuthManager::~AuthManager() {
    if (db) sqlite3_close(db);
}

bool AuthManager::OpenDatabase() {
    int rc = sqlite3_open_v2("D:/RemoteMonitorSystem/Server/Database/users.db", &db,
        SQLITE_OPEN_READONLY, nullptr);
    return rc == SQLITE_OK;
}

bool AuthManager::Authenticate(const std::string& username,
    const std::string& password,
    bool& isAdmin) {
    const char* sql = "SELECT is_admin FROM users "
        "WHERE username = ? AND password = ?";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

    bool authResult = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        isAdmin = sqlite3_column_int(stmt, 0) == 1;
        authResult = true;
    }

    sqlite3_finalize(stmt);
    return authResult;
}