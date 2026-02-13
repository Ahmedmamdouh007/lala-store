#ifndef CONNECTION_H
#define CONNECTION_H

#include <sqlite3.h>
#include <string>

class DatabaseConnection {
public:
    static DatabaseConnection& getInstance();
    sqlite3* getConnection();
    void closeConnection();
    bool isConnected();
    bool ensureSchema();

private:
    DatabaseConnection();
    ~DatabaseConnection();
    DatabaseConnection(const DatabaseConnection&) = delete;
    DatabaseConnection& operator=(const DatabaseConnection&) = delete;

    sqlite3* conn;
    std::string databasePath;

    void loadConfig();
    bool connect();
};

#endif // CONNECTION_H
