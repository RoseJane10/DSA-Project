#include "Item.h"
#include <iostream>

int main() {
    Item product(
        1,
        "Lifter Gloss",
        "Maybelline",
        "Makeup",
        449.00,
        15
    );

    std::cout << "BEAUTRIX ITEM TEST\n";
    std::cout << "Product: " << product.getName() << "\n";
    std::cout << "Brand: " << product.getBrand() << "\n";
    std::cout << "Category: " << product.getCategory() << "\n";
    std::cout << "Price: " << product.getPrice() << "\n";
    std::cout << "Stock: " << product.getStock() << "\n";

    return 0;
}