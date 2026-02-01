#ifndef CONNECTION_H
#define CONNECTION_H

#include <libpq-fe.h>
#include <string>
#include <memory>

class DatabaseConnection {
public:
    static DatabaseConnection& getInstance();
    PGconn* getConnection();
    void closeConnection();
    bool isConnected();
    
private:
    DatabaseConnection();
    ~DatabaseConnection();
    DatabaseConnection(const DatabaseConnection&) = delete;
    DatabaseConnection& operator=(const DatabaseConnection&) = delete;
    
    PGconn* conn;
    std::string host;
    int port;
    std::string database;
    std::string user;
    std::string password;
    
    void loadConfig();
    bool connect();
};

#endif // CONNECTION_H
