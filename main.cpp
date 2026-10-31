#include "Database.h"
#include <iostream>

int main() {
    Database database;

    if (!database.connect()) {
        std::cerr << "PostgreSQL connection failed!\n";
        return 1;
    }

    std::cout << "PostgreSQL connected successfully!\n";

    PGresult* result = database.exec("SELECT version();");

    if (result && PQresultStatus(result) == PGRES_TUPLES_OK) {
        std::cout << "\nDatabase Version:\n";
        std::cout << PQgetvalue(result, 0, 0) << '\n';
    } else {
        std::cerr << "SQL query failed!\n";
    }

    database.finish(result);

    return 0;
}