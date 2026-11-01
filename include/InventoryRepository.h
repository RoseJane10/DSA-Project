
#pragma once

#include "Database.h"
#include "Item.h"
#include "OrderQueue.h"

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

    // Save an order, deduct stock, and calculate its total.
    // All operations succeed together or are rolled back.
    bool processOrder(
        const CustomerOrder& order,
        int& orderId,
        double& orderTotal
    );
};
