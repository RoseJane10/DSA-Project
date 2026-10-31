
#pragma once

#include <string>

class InventoryUndoStack {
public:
    struct UndoAction {
        int productId;
        std::string productName;
        int previousStock;
        int updatedStock;
    };

private:
    struct Node {
        UndoAction action;
        Node* next;

        Node(const UndoAction& action);
    };

    Node* top;
    int size;

public:
    InventoryUndoStack();
    ~InventoryUndoStack();

    // Prevent unsafe copying of dynamically allocated nodes.
    InventoryUndoStack(const InventoryUndoStack&) = delete;
    InventoryUndoStack& operator=(const InventoryUndoStack&) = delete;

    // Add a stock change to the top of the stack.
    void push(const UndoAction& action);

    // Remove and retrieve the most recent stock change.
    bool pop(UndoAction& action);

    bool peek(UndoAction& action) const;
    // Display the most recent action.
    void displayLatest() const;

    bool isEmpty() const;
    int getSize() const;
};
