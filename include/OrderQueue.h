
#pragma once

#include "Item.h"

#include <string>
#include <vector>

// One product inside a customer's order.
struct OrderLineItem {
    Item item;
    int quantity;
};

// Complete customer order.
struct CustomerOrder {
    std::string username;
    std::vector<OrderLineItem> items;
};

// Queue implementation using a singly linked list.
class OrderQueue {
private:
    struct Node {
        CustomerOrder order;
        Node* next;

        explicit Node(const CustomerOrder& order);
    };

    Node* front;
    Node* rear;
    int size;

public:
    OrderQueue();
    ~OrderQueue();

    // Prevent accidental copying of linked-list pointers.
    OrderQueue(const OrderQueue&) = delete;
    OrderQueue& operator=(const OrderQueue&) = delete;

    void enqueue(const CustomerOrder& order);
    bool dequeue(CustomerOrder& order);
    bool peek(CustomerOrder& order) const;

// Remove the front order without copying it.
    bool removeFront();

    void displayQueue() const;

    bool isEmpty() const;
    int getSize() const;
};
