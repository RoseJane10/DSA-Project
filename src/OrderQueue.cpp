
#include "OrderQueue.h"

#include <iostream>
#include <iomanip>
#include <utility>

using namespace std;

OrderQueue::Node::Node(const CustomerOrder& order)
    : order(order), next(nullptr) {
}

OrderQueue::OrderQueue()
    : front(nullptr), rear(nullptr), size(0) {
}

OrderQueue::~OrderQueue() {
    while (front != nullptr) {
        Node* temp = front;
        front = front->next;
        delete temp;
    }

    rear = nullptr;
    size = 0;
}

// Add an order at the rear of the queue.
void OrderQueue::enqueue(const CustomerOrder& order) {
    if (order.username.empty() || order.items.empty()) {
        cout << "\nInvalid order. Cannot add it to the queue.\n";
        return;
    }

    Node* newNode = new Node(order);

    if (isEmpty()) {
        front = newNode;
        rear = newNode;
    } else {
        rear->next = newNode;
        rear = newNode;
    }

    size++;

    cout << "\nOrder successfully added to the checkout queue!\n";
}

// Remove and return the order at the front.
bool OrderQueue::dequeue(CustomerOrder& order) {
    if (isEmpty()) {
        cout << "\nThe checkout queue is empty.\n";
        return false;
    }

    order = front->order;

    Node* temp = front;
    front = front->next;

    delete temp;
    size--;

    if (front == nullptr) {
        rear = nullptr;
    }

    return true;
}

// View the next order without removing it.
bool OrderQueue::peek(CustomerOrder& order) const {
    if (isEmpty()) {
        return false;
    }

    order = front->order;
    return true;
}

// Display all pending orders.
void OrderQueue::displayQueue() const {
    if (isEmpty()) {
        cout << "\nNo pending orders in the queue.\n";
        return;
    }

    Node* current = front;
    int position = 1;

    cout << "\n============================================\n";
    cout << "           CHECKOUT ORDER QUEUE\n";
    cout << "============================================\n";

    while (current != nullptr) {
        cout << "\nQueue Position: " << position << "\n";
        cout << "Customer: " << current->order.username << "\n";

        cout << left
             << setw(25) << "Product"
             << setw(10) << "Quantity"
             << "\n";

        cout << "--------------------------------------------\n";

        for (const OrderLineItem& line : current->order.items) {
            cout << left
                 << setw(25) << line.item.getName()
                 << setw(10) << line.quantity
                 << "\n";
        }

        current = current->next;
        position++;
    }

    cout << "\nTotal pending orders: " << size << "\n";
    cout << "============================================\n";
}

bool OrderQueue::isEmpty() const {
    return front == nullptr;
}

int OrderQueue::getSize() const {
    return size;
}
