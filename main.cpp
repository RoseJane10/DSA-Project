
#include "Database.h"

#include <iostream>
#include <iomanip>

int main() {
    Database database;

    // 1. Connect to PostgreSQL
    if (!database.connect()) {
        std::cerr << "Database connection failed!\n";
        return 1;
    }

    std::cout << "\n============================================\n";
    std::cout << "       BEAUTRIX INVENTORY SYSTEM\n";
    std::cout << "============================================\n";

    // 2. Retrieve products and their brand names
    std::string sql =
        "SELECT p.id, p.name, b.name AS brand, "
        "p.category, p.price, p.stock "
        "FROM products p "
        "JOIN brands b ON p.brand_id = b.id "
        "ORDER BY p.id;";

    PGresult* result = database.exec(sql);

    // 3. Check query result
    if (!result || PQresultStatus(result) != PGRES_TUPLES_OK) {
        std::cerr << "Failed to retrieve products!\n";

        if (result) {
            std::cerr << PQresultErrorMessage(result);
        }

        database.finish(result);
        return 1;
    }

    // 4. Display product table
    std::cout << "\n";
    std::cout << std::left
              << std::setw(5) << "ID"
              << std::setw(32) << "PRODUCT"
              << std::setw(25) << "BRAND"
              << std::setw(15) << "CATEGORY"
              << std::setw(12) << "PRICE"
              << std::setw(8) << "STOCK"
              << "\n";

    std::cout << std::string(97, '-') << "\n";

    for (int i = 0; i < PQntuples(result); i++) {
        std::cout << std::left
                  << std::setw(5) << PQgetvalue(result, i, 0)
                  << std::setw(32) << PQgetvalue(result, i, 1)
                  << std::setw(25) << PQgetvalue(result, i, 2)
                  << std::setw(15) << PQgetvalue(result, i, 3)
                  << std::setw(12) << PQgetvalue(result, i, 4)
                  << std::setw(8) << PQgetvalue(result, i, 5)
                  << "\n";
    }

    std::cout << "\nTotal Products: " << PQntuples(result) << "\n";

    // 5. Release PostgreSQL result
    database.finish(result);

    std::cout << "\n============================================\n";
    std::cout << "       BEAUTRIX DATABASE CONNECTED\n";
    std::cout << "============================================\n";

    return 0;
}
