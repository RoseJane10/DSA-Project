
#include "Database.h"

#include <cstdlib>
#include <iostream>

Database::Database() = default;

Database::~Database() {
    if (connection != nullptr) {
        PQfinish(connection);
        connection = nullptr;
    }
}

bool Database::connect() {
    const char* settings = std::getenv("BEAUTRIX_DB");

    if (settings == nullptr || settings[0] == '\0') {
        std::cerr
            << "BEAUTRIX_DB is not set. "
            << "Check your .env configuration.\n";
        return false;
    }

    // Avoid opening another connection unnecessarily.
    if (ready()) {
        return true;
    }

    // Clean up an old failed connection before reconnecting.
    if (connection != nullptr) {
        PQfinish(connection);
        connection = nullptr;
    }

    connection = PQconnectdb(settings);

    if (connection == nullptr) {
        std::cerr << "Database connection could not be created.\n";
        return false;
    }

    if (PQstatus(connection) != CONNECTION_OK) {
        std::cerr << "Database connection failed: "
                  << PQerrorMessage(connection) << "\n";

        PQfinish(connection);
        connection = nullptr;

        return false;
    }

    std::cout << "Database connected successfully!\n";
    return true;
}

bool Database::ready() const {
    return connection != nullptr &&
           PQstatus(connection) == CONNECTION_OK;
}

PGresult* Database::exec(const std::string& sql) {
    if (!ready()) {
        return nullptr;
    }

    return PQexec(connection, sql.c_str());
}

PGresult* Database::params(
    const std::string& sql,
    int count,
    const char* const values[]
) {
    if (!ready()) {
        return nullptr;
    }

    return PQexecParams(
        connection,
        sql.c_str(),
        count,
        nullptr,
        values,
        nullptr,
        nullptr,
        0
    );
}

void Database::finish(PGresult* result) {
    if (result != nullptr) {
        PQclear(result);
    }
}
