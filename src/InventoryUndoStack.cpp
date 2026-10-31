
#include "InventoryUndoStack.h"
#include <iostream>
using namespace std;

// Create a new stack node.
InventoryUndoStack::Node::Node(const UndoAction& action)
    : action(action), next(nullptr) {}

// Initialize an empty stack.
InventoryUndoStack::InventoryUndoStack()
    : top(nullptr), size(0) {}

// Release all dynamically allocated nodes.
InventoryUndoStack::~InventoryUndoStack() {
    while (top != nullptr) {
        Node* temp = top;
        top = top->next;
        delete temp;
    }

    size = 0;
}

// PUSH: Add a new undo action to the top.
void InventoryUndoStack::push(const UndoAction& action) {
    Node* newNode = new Node(action);

    newNode->next = top;
    top = newNode;

    size++;

    cout << "\nStock change saved to undo stack.\n";
}

// POP: Retrieve and remove the latest undo action.
bool InventoryUndoStack::pop(UndoAction& action) {
    if (isEmpty()) {
        cout << "\nNothing to undo. The stack is empty.\n";
        return false;
    }

    Node* temp = top;

    action = temp->action;

    top = top->next;

    delete temp;
    size--;

    return true;
}

// Display the most recent undo action without removing it.
void InventoryUndoStack::displayLatest() const {
    if (isEmpty()) {
        cout << "\nNo previous stock changes available.\n";
        return;
    }

    cout << "\n========== LATEST UNDO ACTION ==========\n";
    cout << "Product ID: " << top->action.productId << "\n";
    cout << "Product: " << top->action.productName << "\n";
    cout << "Previous Stock: " << top->action.previousStock << "\n";
    cout << "Updated Stock: " << top->action.updatedStock << "\n";
}

bool InventoryUndoStack::isEmpty() const {
    return top == nullptr;
}

int InventoryUndoStack::getSize() const {
    return size;
}

bool InventoryUndoStack::peek(UndoAction& action) const {
    if (isEmpty()) {
        return false;
    }

    action = top->action;
    return true;
}

