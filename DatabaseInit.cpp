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
    // �������ݿ�Ŀ¼
    //fs::create_directory("../Database");

    sqlite3* db;
    int rc = sqlite3_open(dbPath, &db);
    if (rc != SQLITE_OK) {
        std::cerr << "�޷��������ݿ�: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // �����û���
    const char* sql =
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT UNIQUE NOT NULL,"
        "password TEXT NOT NULL,"
        "is_admin INTEGER DEFAULT 0);";

    rc = sqlite3_exec(db, sql, 0, 0, 0);
    if (rc != SQLITE_OK) {
        std::cerr << "������ʧ��: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return false;
    }

    // �����ʼ�û�
    const char* insertSql =
        "INSERT OR IGNORE INTO users (username, password, is_admin) VALUES "
        "('admin', 'admin123', 1),"
        "('user1', 'user123', 0);";

    rc = sqlite3_exec(db, insertSql, 0, 0, 0);
    if (rc != SQLITE_OK) {
        std::cerr << "��ʼ������ʧ��: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return false;
    }

    sqlite3_close(db);
    std::cout << "���ݿ��ʼ���ɹ���" << std::endl;
    return true;
}

// �ڷ��������ʱ���ô˺���
// InitializeDatabase();