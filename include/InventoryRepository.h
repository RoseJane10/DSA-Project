
#pragma once

#include "Database.h"
#include "Item.h"

#include <vector>

class InventoryRepository {
private:
    Database& database;

public:

    InventoryRepository(Database& database);

    // Retrieve all products from PostgreSQL.
    std::vector<Item> getAllProducts();

    // Update stock only if the current stock matches the expected value.
    bool updateProductStock(
        int productId,
        int expectedStock,
        int newStock
    );
};
