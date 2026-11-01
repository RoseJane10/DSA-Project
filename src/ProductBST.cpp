
#include "ProductBST.h"

#include <algorithm>
#include <utility>

// Constructor.
ProductBST::ProductBST()
    : root(nullptr), size(0) {}

// Insert a product recursively.
bool ProductBST::insertNode(
    std::unique_ptr<Node>& node,
    const Item& product
) {

    if (node == nullptr) {

        node = std::make_unique<Node>(product);

        return true;
    }

    if (product.getId() < node->item.getId()) {

        return insertNode(node->left, product);
    }

    if (product.getId() > node->item.getId()) {

        return insertNode(node->right, product);
    }

    // Duplicate product ID.
    return false;
}

// Public insert function.
bool ProductBST::insert(const Item& product) {

    if (product.getId() <= 0) {
        return false;
    }

    if (insertNode(root, product)) {

        size++;

        return true;
    }

    return false;
}

// Build a balanced BST recursively.
std::unique_ptr<ProductBST::Node>
ProductBST::buildBalanced(
    const std::vector<Item>& products,
    int start,
    int end
) {

    if (start > end) {
        return nullptr;
    }

    int middle = start + (end - start) / 2;

    auto node = std::make_unique<Node>(products[middle]);

    node->left = buildBalanced(
        products,
        start,
        middle - 1
    );

    node->right = buildBalanced(
        products,
        middle + 1,
        end
    );

    return node;
}

// Rebuild the BST from the current inventory.
bool ProductBST::build(const std::vector<Item>& products) {

    std::vector<Item> sortedProducts = products;

    // Sort products by ID.
    std::sort(
        sortedProducts.begin(),
        sortedProducts.end(),
        [](const Item& a, const Item& b) {
            return a.getId() < b.getId();
        }
    );

    // Validate product IDs and reject duplicates.
    for (std::size_t i = 0; i < sortedProducts.size(); i++) {

        if (sortedProducts[i].getId() <= 0) {
            return false;
        }

        if (i > 0 &&
            sortedProducts[i - 1].getId() ==
            sortedProducts[i].getId()) {

            return false;
        }
    }

    // Build a temporary tree first.
    ProductBST rebuilt;

    if (!sortedProducts.empty()) {

        rebuilt.root = rebuilt.buildBalanced(
            sortedProducts,
            0,
            static_cast<int>(sortedProducts.size()) - 1
        );
    }

    rebuilt.size = sortedProducts.size();

    // Replace the old tree only after successful construction.
    root.swap(rebuilt.root);

    std::swap(size, rebuilt.size);

    return true;
}

// Recursive BST search.
const Item* ProductBST::searchNode(
    const Node* node,
    int productId
) const {

    if (node == nullptr) {
        return nullptr;
    }

    if (productId == node->item.getId()) {
        return &node->item;
    }

    if (productId < node->item.getId()) {

        return searchNode(node->left.get(), productId);
    }

    return searchNode(node->right.get(), productId);
}

// Public search function.
const Item* ProductBST::search(int productId) const {

    if (productId <= 0) {
        return nullptr;
    }

    return searchNode(root.get(), productId);
}

// Check whether BST is empty.
bool ProductBST::isEmpty() const {

    return root == nullptr;
}

// Get number of products in BST.
std::size_t ProductBST::getSize() const {

    return size;
}
