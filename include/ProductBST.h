
#pragma once

#include "Item.h"

#include <memory>
#include <vector>
#include <cstddef>

class ProductBST {
private:

    struct Node {
        Item item;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;

        explicit Node(const Item& product)
            : item(product), left(nullptr), right(nullptr) {}
    };

    std::unique_ptr<Node> root;
    std::size_t size;

    // Insert a product into the BST.
    bool insertNode(
        std::unique_ptr<Node>& node,
        const Item& product
    );

    // Build a balanced BST from sorted products.
    std::unique_ptr<Node> buildBalanced(
        const std::vector<Item>& products,
        int start,
        int end
    );

    // Search recursively using product ID.
    const Item* searchNode(
        const Node* node,
        int productId
    ) const;

public:

    ProductBST();

    // Prevent copying of the BST.
    ProductBST(const ProductBST&) = delete;
    ProductBST& operator=(const ProductBST&) = delete;

    // Insert one product.
    bool insert(const Item& product);

    // Rebuild the BST using the current inventory.
    bool build(const std::vector<Item>& products);

    // Search for a product using its ID.
    const Item* search(int productId) const;

    bool isEmpty() const;
    std::size_t getSize() const;
};
