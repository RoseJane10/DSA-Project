
#include "DSAComponents.h"

#include <algorithm>
#include <cctype>

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
}
