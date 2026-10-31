
#pragma once

#include "Database.h"
#include "Item.h"

#include <vector>

class InventoryRepository {
private:
    Database& database;

public:
    InventoryRepository(Database& database);

    std::vector<Item> getAllProducts();
};
