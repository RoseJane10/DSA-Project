
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
