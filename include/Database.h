#pragma once
#include <libpq-fe.h>
#include <string>

class Database {
    PGconn* connection = nullptr;
public:
    Database();
    ~Database();
    bool connect();
    bool ready() const;
    PGresult* exec(const std::string& sql);
    PGresult* params(const std::string& sql, int count, const char* const values[]);
    void finish(PGresult* result);
};
