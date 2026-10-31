
#pragma once

#include "Item.h"

class ShoppingCart {

private:

    // Doubly Linked List Node
    struct Node {
        Item item;
        int quantity;

        Node* prev;
        Node* next;

        Node(const Item& item, int quantity);
    };

    Node* head;
    Node* tail;
    Node* current;

    int size;

public:

    ShoppingCart();
    ~ShoppingCart();

    // Prevent unsafe copying of raw pointers.
    ShoppingCart(const ShoppingCart&) = delete;
    ShoppingCart& operator=(const ShoppingCart&) = delete;

    void addItem(const Item& item, int quantity);
    bool removeItem(int productId);
    bool updateQuantity(int productId, int quantity);

    void displayCart() const;
    void displayForward();
    void displayBackward();

    double calculateTotal() const;

    bool isEmpty() const;
    int getSize() const;
};
