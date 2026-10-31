
#include "Database.h"
#include "InventoryRepository.h"

#include <iostream>
#include <iomanip>

int main() {

    Database database;

    if (!database.connect()) {
        return 1;
    }

    InventoryRepository repository(database);

    std::vector<Item> products = repository.getAllProducts();

    std::cout << "\n============================================\n";
    std::cout << "          BEAUTRIX INVENTORY SYSTEM\n";
    std::cout << "============================================\n\n";

    std::cout << std::left
              << std::setw(5) << "ID"
              << std::setw(38) << "PRODUCT"
              << std::setw(32) << "BRAND"
              << std::setw(15) << "CATEGORY"
              << std::setw(12) << "PRICE"
              << "STOCK\n";

    std::cout << std::string(102, '-') << "\n";

    for (const Item& product : products) {

        std::cout << std::left
                  << std::setw(5) << product.getId()
                  << std::setw(38) << product.getName()
                  << std::setw(32) << product.getBrand()
                  << std::setw(15) << product.getCategory()
                  << std::setw(12) << product.getPrice()
                  << product.getStock()
                  << "\n";
    }

    std::cout << "\nTotal Products: " << products.size() << "\n";

    std::cout << "\n============================================\n";
    std::cout << "          BEAUTRIX DATABASE CONNECTED\n";
    std::cout << "============================================\n";

    return 0;
}
