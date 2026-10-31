
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
} // IMPORTANT: Close Linear Search here!

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
