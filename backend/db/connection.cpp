#include "connection.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

DatabaseConnection& DatabaseConnection::getInstance() {
    static DatabaseConnection instance;
    return instance;
}

DatabaseConnection::DatabaseConnection() : conn(nullptr) {
    loadConfig();
    connect();
}

DatabaseConnection::~DatabaseConnection() {
    closeConnection();
}

void DatabaseConnection::loadConfig() {
    const std::vector<std::string> configPaths = {
        "../config/db_config.json",
        "config/db_config.json",
        "backend/config/db_config.json",
    };
    std::ifstream configFile;
    for (const auto& path : configPaths) {
        configFile.open(path);
        if (configFile.is_open())
            break;
    }
    if (configFile.is_open()) {
        json config;
        configFile >> config;
        if (config.contains("database_path") && config["database_path"].is_string())
            databasePath = config["database_path"].get<std::string>();
        configFile.close();
    }
    if (databasePath.empty()) {
        databasePath = "database/lala-store.db";
    }
}

static int tryOpenDb(const std::string& path, sqlite3** out) {
    return sqlite3_open(path.c_str(), out);
}

bool DatabaseConnection::connect() {
    // Try paths relative to CWD: project root, backend/, or build/Release
    std::vector<std::string> tryPaths = {
        databasePath,
        "../" + databasePath,
        "../../" + databasePath,
        "../../../" + databasePath,
        "backend/" + databasePath,
    };
    int rc = SQLITE_OK;
    for (const auto& path : tryPaths) {
        rc = tryOpenDb(path, &conn);
        if (rc == SQLITE_OK && conn != nullptr) {
            databasePath = path;
            break;
        }
        if (conn) {
            sqlite3_close(conn);
            conn = nullptr;
        }
    }
    if (rc != SQLITE_OK || conn == nullptr) {
        std::cerr << "SQLite open failed: " << (conn ? sqlite3_errmsg(conn) : "out of memory") << std::endl;
        if (conn) {
            sqlite3_close(conn);
            conn = nullptr;
        }
        return false;
    }
    sqlite3_exec(conn, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
    std::cout << "Connected to SQLite database: " << databasePath << std::endl;
    return true;
}

bool DatabaseConnection::ensureSchema() {
    const std::vector<std::string> schemaPaths = {
        "database/schema_sqlite.sql",
        "../database/schema_sqlite.sql",
        "../../database/schema_sqlite.sql",
        "../../../database/schema_sqlite.sql",
        "backend/../database/schema_sqlite.sql",
    };
    std::ifstream schemaFile;
    for (const auto& path : schemaPaths) {
        schemaFile.open(path);
        if (schemaFile.is_open())
            break;
    }
    if (!schemaFile.is_open())
        return true;
    std::stringstream buf;
    buf << schemaFile.rdbuf();
    schemaFile.close();
    std::string sql = buf.str();
    char* errMsg = nullptr;
    int rc = sqlite3_exec(conn, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK && errMsg) {
        std::cerr << "Schema exec error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    if (errMsg) sqlite3_free(errMsg);
    return true;
}

bool DatabaseConnection::isConnected() {
    return conn != nullptr;
}

sqlite3* DatabaseConnection::getConnection() {
    if (!conn && !connect()) return nullptr;
    return conn;
}

void DatabaseConnection::closeConnection() {
    if (conn != nullptr) {
        sqlite3_close(conn);
        conn = nullptr;
    }
}
