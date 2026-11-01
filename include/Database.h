
#pragma once

#include <libpq-fe.h>
#include <string>

class Database {
private:
    PGconn* connection = nullptr;

public:
    Database();
    ~Database();

    // Prevent copying a database connection.
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    bool connect();
    bool ready() const;

    // Execute ordinary SQL commands.
    PGresult* exec(const std::string& sql);

    // Execute parameterized SQL queries.
    PGresult* params(
        const std::string& sql,
        int count,
        const char* const values[]
    );

    // Release PostgreSQL query results.
    void finish(PGresult* result);
};
