#pragma once

#include "Item.h"

#include <cstddef>
#include <memory>
#include <vector>

class DynamicProductCatalog {
private:
    std::unique_ptr<Item>* data;
    std::size_t size;
    std::size_t capacity;

    void resize(std::size_t newCapacity);

public:
    // Constructor: creates the initial dynamic array.
    explicit DynamicProductCatalog(std::size_t initialCapacity = 4);

    // Destructor: releases dynamically allocated memory.
    ~DynamicProductCatalog();

    // Prevent accidental copying and double ownership.
    DynamicProductCatalog(const DynamicProductCatalog&) = delete;
    DynamicProductCatalog& operator=(const DynamicProductCatalog&) = delete;

    // Disable moving to keep ownership simple.
    DynamicProductCatalog(DynamicProductCatalog&&) = delete;
    DynamicProductCatalog& operator=(DynamicProductCatalog&&) = delete;

    // Add a product and resize automatically when full.
    void add(const Item& item);

    // Load products from PostgreSQL.
    void load(const std::vector<Item>& products);

    // Clear products while retaining allocated capacity.
    void clear();

    // Retrieve a product by index.
    const Item* get(std::size_t index) const;

    // Convert the catalog into a vector for existing features.
    std::vector<Item> toVector() const;

    // Get catalog information.
    std::size_t getSize() const;
    std::size_t getCapacity() const;
    bool isEmpty() const;
};