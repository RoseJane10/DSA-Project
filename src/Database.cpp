// DATABASE MODULE - PostgreSQL persistence for catalog and checkout operations.
#include "Database.h"
#include <cstdlib>
#include <iostream>

Database::Database() = default;
Database::~Database() {
    if (connection) PQfinish(connection);
}
bool Database::connect() {
    const char* settings = std::getenv("BEAUTRIX_DB");
    if (!settings) {
        std::cerr << "BEAUTRIX_DB is not set. Set it to your PostgreSQL connection string.\n";
        return false;
    }
    connection = PQconnectdb(settings);
    if (!connection || PQstatus(connection) != CONNECTION_OK) {
        std::cerr << "Database connection failed: "
                  << (connection ? PQerrorMessage(connection) : "No connection") << "\n";
        return false;
    }
    return true;
}
bool Database::ready() const {
    return connection && PQstatus(connection) == CONNECTION_OK;
}
PGresult* Database::exec(const std::string& sql) {
    if (!ready()) return nullptr;
    return PQexec(connection, sql.c_str());
}
PGresult* Database::params(const std::string& sql, int count, const char* const values[]) {
    if (!ready()) return nullptr;
    return PQexecParams(connection, sql.c_str(), count, nullptr, values, nullptr, nullptr, 0);
}
void Database::finish(PGresult* result) {
    if (result) PQclear(result);
}
