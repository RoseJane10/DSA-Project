
#include "DSAComponents.h"

#include <algorithm>
#include <cctype>
#include <cmath>

// Convert text to lowercase for case-insensitive searching.
static std::string toLowerCase(const std::string& text) {

    std::string result = text;

    std::transform(
        result.begin(),
        result.end(),
        result.begin(),
        [](unsigned char character) {
            return static_cast<char>(std::tolower(character));
        }
    );

    return result;
}

// Lab 7: Linear Search
std::vector<Item> linearSearchProducts(
    const std::vector<Item>& products,
    const std::string& keyword
) {

    std::vector<Item> results;

    std::string searchKeyword = toLowerCase(keyword);

    if (searchKeyword.empty()) {
        return results;
    }

    // Check every product one by one.
    for (const Item& product : products) {

        std::string productName =
            toLowerCase(product.getName());

        std::string brandName =
            toLowerCase(product.getBrand());

        // Match either the product name or brand.
        if (productName.find(searchKeyword) != std::string::npos ||
            brandName.find(searchKeyword) != std::string::npos) {

            results.push_back(product);
        }
    }

    return results;
}

// Official Brand Filtering
std::vector<Item> filterProductsByBrand(
    const std::vector<Item>& products,
    const std::string& brand
) {

    std::vector<Item> results;

    std::string selectedBrand = toLowerCase(brand);

    if (selectedBrand.empty()) {
        return results;
    }

    // Check every product for an exact brand match.
    for (const Item& product : products) {

        std::string productBrand =
            toLowerCase(product.getBrand());

        if (productBrand == selectedBrand) {
            results.push_back(product);
        }
    }

    return results;
}

// Lab 7: Binary Search for Price Range
std::vector<Item> binarySearchPriceRange(
    const std::vector<Item>& products,
    double minPrice,
    double maxPrice
) {

    std::vector<Item> results;

    // Reject invalid price ranges.
    if (!std::isfinite(minPrice) ||
        !std::isfinite(maxPrice) ||
        minPrice < 0 ||
        maxPrice < minPrice) {

        return results;
    }

    // Make a copy so the original inventory order is unchanged.
    std::vector<Item> sortedProducts = products;

    // Sort products by price from lowest to highest.
    std::sort(
        sortedProducts.begin(),
        sortedProducts.end(),
        [](const Item& a, const Item& b) {
            return a.getPrice() < b.getPrice();
        }
    );

    // Binary Search: find the first product with price >= minPrice.
    int left = 0;
    int right = static_cast<int>(sortedProducts.size());

    while (left < right) {

        int mid = left + (right - left) / 2;

        if (sortedProducts[mid].getPrice() < minPrice) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }

    int firstIndex = left;

    // Binary Search: find the first product with price > maxPrice.
    left = 0;
    right = static_cast<int>(sortedProducts.size());

    while (left < right) {

        int mid = left + (right - left) / 2;

        if (sortedProducts[mid].getPrice() <= maxPrice) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }

    int lastIndex = left;

    // Collect every product within the inclusive price range.
    for (int i = firstIndex; i < lastIndex; i++) {
        results.push_back(sortedProducts[i]);
    }

    return results;
}

// Quick Sort partition helper.
template <typename Compare>
int partitionItems(
    std::vector<Item>& products,
    int low,
    int high,
    Compare comesBefore
) {

    Item pivot = products[high];

    int i = low - 1;

    for (int j = low; j < high; j++) {

        if (comesBefore(products[j], pivot)) {

            i++;

            std::swap(products[i], products[j]);
        }
    }

    std::swap(products[i + 1], products[high]);

    return i + 1;
}

// Recursive Quick Sort helper.
template <typename Compare>
void quickSortItems(
    std::vector<Item>& products,
    int low,
    int high,
    Compare comesBefore
) {

    if (low < high) {

        int pivotIndex = partitionItems(
            products, low, high, comesBefore
        );

        quickSortItems(
            products, low, pivotIndex - 1, comesBefore
        );

        quickSortItems(
            products, pivotIndex + 1, high, comesBefore
        );
    }
}

// Lab 8: Insertion Sort by Brand
std::vector<Item> insertionSortByBrand(
    const std::vector<Item>& products
) {

    std::vector<Item> sortedProducts = products;

    for (int i = 1; i < static_cast<int>(sortedProducts.size()); i++) {

        Item key = sortedProducts[i];

        std::string keyBrand = toLowerCase(key.getBrand());

        int j = i - 1;

        while (j >= 0) {

            std::string previousBrand =
                toLowerCase(sortedProducts[j].getBrand());

            bool shouldMove =
                previousBrand > keyBrand ||
                (previousBrand == keyBrand &&
                 sortedProducts[j].getId() > key.getId());

            if (!shouldMove) {
                break;
            }

            sortedProducts[j + 1] = sortedProducts[j];

            j--;
        }

        sortedProducts[j + 1] = key;
    }

    return sortedProducts;
}

// Lab 8: Quick Sort by Price
std::vector<Item> quickSortByPrice(
    const std::vector<Item>& products
) {

    std::vector<Item> sortedProducts = products;

    if (!sortedProducts.empty()) {

        quickSortItems(
            sortedProducts,
            0,
            static_cast<int>(sortedProducts.size()) - 1,
            [](const Item& a, const Item& b) {

                if (a.getPrice() == b.getPrice()) {
                    return a.getId() < b.getId();
                }

                return a.getPrice() < b.getPrice();
            }
        );
    }

    return sortedProducts;
}

// Lab 8: Quick Sort by Stock
std::vector<Item> quickSortByStock(
    const std::vector<Item>& products
) {

    std::vector<Item> sortedProducts = products;

    if (!sortedProducts.empty()) {

        quickSortItems(
            sortedProducts,
            0,
            static_cast<int>(sortedProducts.size()) - 1,
            [](const Item& a, const Item& b) {

                if (a.getStock() == b.getStock()) {
                    return a.getId() < b.getId();
                }

                return a.getStock() < b.getStock();
            }
        );
    }

    return sortedProducts;
}
