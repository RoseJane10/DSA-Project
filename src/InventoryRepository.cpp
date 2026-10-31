
#include "InventoryRepository.h"

#include <iostream>
#include <string>

InventoryRepository::InventoryRepository(Database& database)
    : database(database) {
}

std::vector<Item> InventoryRepository::getAllProducts() {

    std::vector<Item> products;

    std::string sql =
        "SELECT p.id, p.name, b.name AS brand, "
        "p.category, p.price, p.stock "
        "FROM products p "
        "JOIN brands b ON p.brand_id = b.id "
        "ORDER BY p.id;";

    PGresult* result = database.exec(sql);

    if (!result || PQresultStatus(result) != PGRES_TUPLES_OK) {

        std::cerr << "Failed to retrieve products!\n";

        if (result) {
            std::cerr << PQresultErrorMessage(result);
        }

        database.finish(result);
        return products;
    }

    for (int i = 0; i < PQntuples(result); i++) {

        int id = std::stoi(PQgetvalue(result, i, 0));
        std::string name = PQgetvalue(result, i, 1);
        std::string brand = PQgetvalue(result, i, 2);
        std::string category = PQgetvalue(result, i, 3);
        double price = std::stod(PQgetvalue(result, i, 4));
        int stock = std::stoi(PQgetvalue(result, i, 5));

        products.emplace_back(
            id, name, brand, category, price, stock
        );
    }

    database.finish(result);

    return products;
}

// Update product stock in PostgreSQL.
bool InventoryRepository::updateProductStock(
    int productId,
    int expectedStock,
    int newStock
) {

    if (productId <= 0 || expectedStock < 0 || newStock < 0) {
        std::cout << "\nInvalid product ID or stock quantity.\n";
        return false;
    }

    std::string sql =
        "UPDATE products "
        "SET stock = $3 "
        "WHERE id = $1 AND stock = $2 "
        "RETURNING id;";

    std::string idValue = std::to_string(productId);
    std::string expectedValue = std::to_string(expectedStock);
    std::string newValue = std::to_string(newStock);

    const char* values[] = {
        idValue.c_str(),
        expectedValue.c_str(),
        newValue.c_str()
    };

    PGresult* result = database.params(sql, 3, values);

    if (!result || PQresultStatus(result) != PGRES_TUPLES_OK) {

        std::cerr << "\nFailed to update product stock!\n";

        if (result) {
            std::cerr << PQresultErrorMessage(result);
        }

        database.finish(result);
        return false;
    }

    bool updated = PQntuples(result) == 1;

    database.finish(result);

    if (!updated) {
        std::cout
            << "\nStock update failed. The product may not exist "
            << "or its stock may have changed.\n";
        return false;
    }

    return true;
}
