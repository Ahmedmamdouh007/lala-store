#ifndef USER_H
#define USER_H

#include <string>

struct User {
    int id;
    std::string username;
    std::string email;
    std::string password_hash;
    std::string created_at;
};

#endif // USER_H
