
#include "DynamicProductCatalog.h"

#include <iostream>
#include <limits>
#include <stdexcept>
#include <utility>

// Constructor: allocate the initial array.
DynamicProductCatalog::DynamicProductCatalog(std::size_t initialCapacity)
    : data(nullptr), size(0), capacity(initialCapacity) {

    if (capacity == 0) {
        capacity = 4;
    }

    data = new std::unique_ptr<Item>[capacity];
}

// Destructor: release the dynamically allocated array.
// Each unique_ptr automatically deletes its Item.
DynamicProductCatalog::~DynamicProductCatalog() {
    delete[] data;
}

// Resize the array when it becomes full.
void DynamicProductCatalog::resize(std::size_t newCapacity) {
    if (newCapacity <= capacity) {
        return;
    }

    // Allocate a larger array.
    std::unique_ptr<Item>* newData =
        new std::unique_ptr<Item>[newCapacity];

    // Move existing products into the new array.
    for (std::size_t i = 0; i < size; i++) {
        newData[i] = std::move(data[i]);
    }

    // Release the old array.
    delete[] data;

    data = newData;
    capacity = newCapacity;

    std::cout << "\nCatalog memory resized successfully!\n";
    std::cout << "New capacity: " << capacity << " products\n";
}

// Add a product to the catalog.
void DynamicProductCatalog::add(const Item& item) {
    if (size == capacity) {
        if (capacity > std::numeric_limits<std::size_t>::max() / 2) {
            throw std::length_error("Catalog capacity limit reached.");
        }

        resize(capacity * 2);
    }

    data[size] = std::make_unique<Item>(item);
    size++;
}

// Load products from a vector.
void DynamicProductCatalog::load(const std::vector<Item>& products) {
    clear();

    for (const Item& item : products) {
        add(item);
    }
}

// Remove all products but retain allocated capacity.
void DynamicProductCatalog::clear() {
    for (std::size_t i = 0; i < size; i++) {
        data[i].reset();
    }

    size = 0;
}

// Retrieve a product using its index.
const Item* DynamicProductCatalog::get(std::size_t index) const {
    if (index >= size) {
        return nullptr;
    }

    return data[index].get();
}

// Convert the dynamic catalog into a vector.
std::vector<Item> DynamicProductCatalog::toVector() const {
    std::vector<Item> products;
    products.reserve(size);

    for (std::size_t i = 0; i < size; i++) {
        products.push_back(*data[i]);
    }

    return products;
}

// Return the number of products.
std::size_t DynamicProductCatalog::getSize() const {
    return size;
}

// Return the allocated capacity.
std::size_t DynamicProductCatalog::getCapacity() const {
    return capacity;
}

// Check whether the catalog is empty.
bool DynamicProductCatalog::isEmpty() const {
    return size == 0;
}
