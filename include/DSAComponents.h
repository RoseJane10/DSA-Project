
#pragma once

#include "Item.h"

#include <string>
#include <vector>

// Lab 7: Linear Search
std::vector<Item> linearSearchProducts(
    const std::vector<Item>& products,
    const std::string& keyword
);

// Lab 7: Binary Search for Price Range
std::vector<Item> binarySearchPriceRange(
    const std::vector<Item>& products,
    double minPrice,
    double maxPrice
);