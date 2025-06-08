// Server/src/Auth/DatabaseInit.cpp
#include "pch.h"
#include <sqlite3.h>
#include <iostream>
#include <filesystem>
#include "DatabaseInit.h"

namespace fs = std::filesystem;

bool InitializeDatabase() {
    //const char* dbPath = "../Database/users.db";
    const char* dbPath = "D:/RemoteMonitorSystem/Server/Database/users.db";
    fs::create_directories("D:/RemoteMonitorSystem/Server/Database");
    // 创建数据库目录
    //fs::create_directory("../Database");

    sqlite3* db;
    int rc = sqlite3_open(dbPath, &db);
    if (rc != SQLITE_OK) {
        std::cerr << "无法创建数据库: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // 创建用户表
    const char* sql =
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT UNIQUE NOT NULL,"
        "password TEXT NOT NULL,"
        "is_admin INTEGER DEFAULT 0);";

    rc = sqlite3_exec(db, sql, 0, 0, 0);
    if (rc != SQLITE_OK) {
        std::cerr << "创建表失败: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return false;
    }

    // 插入初始用户
    const char* insertSql =
        "INSERT OR IGNORE INTO users (username, password, is_admin) VALUES "
        "('admin', 'admin123', 1),"
        "('user1', 'user123', 0);";

    rc = sqlite3_exec(db, insertSql, 0, 0, 0);
    if (rc != SQLITE_OK) {
        std::cerr << "初始化数据失败: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return false;
    }

    sqlite3_close(db);
    std::cout << "数据库初始化成功！" << std::endl;
    return true;
}

// 在服务端启动时调用此函数
// InitializeDatabase();