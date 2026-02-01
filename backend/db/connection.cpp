#include "connection.h"
#include <fstream>
#include <iostream>
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
    std::ifstream configFile("../config/db_config.json");
    if (configFile.is_open()) {
        json config;
        configFile >> config;
        host = config["host"].get<std::string>();
        port = config["port"].get<int>();
        database = config["database"].get<std::string>();
        user = config["user"].get<std::string>();
        password = config["password"].get<std::string>();
    } else {
        // Fallback to hardcoded values
        host = "localhost";
        port = 5432;
        database = "Lala store";
        user = "postgres";
        password = "1234";
    }
}

bool DatabaseConnection::connect() {
    // Build connection string - escape database name if it contains spaces
    std::string dbname = database;
    // PostgreSQL connection string format: key=value pairs, space in value needs quoting
    std::string connString = "host=" + host + 
                            " port=" + std::to_string(port) + 
                            " dbname='" + database + "'" +
                            " user=" + user + 
                            " password=" + password;
    
    conn = PQconnectdb(connString.c_str());
    
    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Connection to database failed: " << PQerrorMessage(conn) << std::endl;
        PQfinish(conn);
        conn = nullptr;
        return false;
    }
    
    std::cout << "Connected to PostgreSQL database successfully!" << std::endl;
    return true;
}

PGconn* DatabaseConnection::getConnection() {
    if (!isConnected()) {
        connect();
    }
    return conn;
}

bool DatabaseConnection::isConnected() {
    return conn != nullptr && PQstatus(conn) == CONNECTION_OK;
}

void DatabaseConnection::closeConnection() {
    if (conn != nullptr) {
        PQfinish(conn);
        conn = nullptr;
    }
}
