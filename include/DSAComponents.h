
#pragma once

#include "Item.h"

#include <string>
#include <vector>

// Lab 7: Linear Search
std::vector<Item> linearSearchProducts(
    const std::vector<Item>& products,
    const std::string& keyword
);

// Official Brand Filtering
std::vector<Item> filterProductsByBrand(
    const std::vector<Item>& products,
    const std::string& brand
);

// Lab 7: Binary Search for Price Range
std::vector<Item> binarySearchPriceRange(
    const std::vector<Item>& products,
    double minPrice,
    double maxPrice
);

// Lab 8: Insertion Sort by Brand
std::vector<Item> insertionSortByBrand(
    const std::vector<Item>& products
);

// Lab 8: Quick Sort by Price
std::vector<Item> quickSortByPrice(
    const std::vector<Item>& products
);

// Lab 8: Quick Sort by Stock
std::vector<Item> quickSortByStock(
    const std::vector<Item>& products
);
