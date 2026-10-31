
#pragma once

#include "Database.h"
#include <string>

struct AuthenticatedUser {
    int id;
    std::string username;
    std::string role;
};

class AuthSystem {
private:
    Database& database;

public:
    explicit AuthSystem(Database& database);

    bool login(AuthenticatedUser& user);
};
