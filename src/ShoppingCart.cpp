
#include "ShoppingCart.h"

#include <iostream>
#include <iomanip>
#include <limits>

using namespace std;

// Create a new doubly linked list node.
ShoppingCart::Node::Node(const Item& item, int quantity)
    : item(item),
      quantity(quantity),
      prev(nullptr),
      next(nullptr) {
}

// Initialize an empty shopping cart.
ShoppingCart::ShoppingCart()
    : head(nullptr),
      tail(nullptr),
      current(nullptr),
      size(0) {
}

// Free all dynamically allocated nodes.
ShoppingCart::~ShoppingCart() {
    Node* temp = head;

    while (temp != nullptr) {
        Node* nextNode = temp->next;
        delete temp;
        temp = nextNode;
    }

    head = nullptr;
    tail = nullptr;
    current = nullptr;
    size = 0;
}

// Add an item to the cart.
void ShoppingCart::addItem(const Item& item, int quantity) {

    if (quantity <= 0) {
        cout << "Invalid quantity. Enter a quantity greater than zero.\n";
        return;
    }

    if (quantity > item.getStock()) {
        cout << "Not enough stock available.\n";
        return;
    }

    // Check whether the product already exists.
    Node* temp = head;

    while (temp != nullptr) {

        if (temp->item.getId() == item.getId()) {

            if (quantity > item.getStock() - temp->quantity) {
                cout << "Cannot add more. Maximum available stock reached.\n";
                return;
            }

            temp->quantity += quantity;

            cout << "Item quantity updated successfully!\n";
            return;
        }

        temp = temp->next;
    }

    // Create a new node.
    Node* newNode = new Node(item, quantity);

    if (head == nullptr) {
        head = newNode;
        tail = newNode;
        current = newNode;
    }
    else {
        tail->next = newNode;
        newNode->prev = tail;
        tail = newNode;
    }

    size++;

    cout << "Item added to shopping cart!\n";
}

// Remove an item using its product ID.
bool ShoppingCart::removeItem(int productId) {

    Node* temp = head;

    while (temp != nullptr) {

        if (temp->item.getId() == productId) {

            if (temp->prev != nullptr) {
                temp->prev->next = temp->next;
            }
            else {
                head = temp->next;
            }

            if (temp->next != nullptr) {
                temp->next->prev = temp->prev;
            }
            else {
                tail = temp->prev;
            }

            if (current == temp) {
                current = (temp->next != nullptr)
                    ? temp->next
                    : temp->prev;
            }

            delete temp;
            size--;

            if (head == nullptr) {
                tail = nullptr;
                current = nullptr;
            }

            cout << "Item removed successfully!\n";
            return true;
        }

        temp = temp->next;
    }

    cout << "Product ID not found in cart.\n";
    return false;
}

// Change the quantity of an existing cart item.
bool ShoppingCart::updateQuantity(int productId, int quantity) {

    if (quantity <= 0) {
        cout << "Invalid quantity. Enter at least 1.\n";
        return false;
    }

    Node* temp = head;

    while (temp != nullptr) {

        if (temp->item.getId() == productId) {

            if (quantity > temp->item.getStock()) {
                cout << "Requested quantity exceeds available stock.\n";
                return false;
            }

            temp->quantity = quantity;

            cout << "Quantity updated successfully!\n";
            return true;
        }

        temp = temp->next;
    }

    cout << "Product ID not found in cart.\n";
    return false;
}

// Display all cart items.
void ShoppingCart::displayCart() const {

    if (isEmpty()) {
        cout << "\nYour shopping cart is empty.\n";
        return;
    }

    cout << "\n================ SHOPPING CART ================\n";

    cout << left
         << setw(8) << "ID"
         << setw(25) << "PRODUCT"
         << setw(18) << "BRAND"
         << setw(10) << "QTY"
         << setw(12) << "PRICE"
         << setw(12) << "SUBTOTAL"
         << "\n";

    cout << string(85, '-') << "\n";

    Node* temp = head;

    while (temp != nullptr) {

        double subtotal =
            temp->item.getPrice() * temp->quantity;

        cout << left
             << setw(8) << temp->item.getId()
             << setw(25) << temp->item.getName().substr(0, 23)
             << setw(18) << temp->item.getBrand().substr(0, 16)
             << setw(10) << temp->quantity
             << "P" << setw(11) << fixed << setprecision(2)
             << temp->item.getPrice()
             << "P" << fixed << setprecision(2)
             << subtotal
             << "\n";

        temp = temp->next;
    }

    cout << string(85, '-') << "\n";

    cout << "Total Items: " << size << "\n";
    cout << "Grand Total: P"
         << fixed << setprecision(2)
         << calculateTotal() << "\n";
}

// Navigate forward through the doubly linked list.
void ShoppingCart::displayForward() {

    if (isEmpty()) {
        cout << "Shopping cart is empty.\n";
        return;
    }

    if (current == nullptr) {
        current = head;
    }

    cout << "\n--- FORWARD NAVIGATION ---\n";
    cout << "Product: " << current->item.getName() << "\n";
    cout << "Brand: " << current->item.getBrand() << "\n";
    cout << "Quantity: " << current->quantity << "\n";

    current = current->next;

    if (current == nullptr) {
        cout << "You have reached the end of the cart.\n";
    }
}

// Navigate backward through the doubly linked list.
void ShoppingCart::displayBackward() {

    if (isEmpty()) {
        cout << "Shopping cart is empty.\n";
        return;
    }

    if (current == nullptr) {
        current = tail;
    }
    else if (current->prev != nullptr) {
        current = current->prev;
    }

    cout << "\n--- BACKWARD NAVIGATION ---\n";
    cout << "Product: " << current->item.getName() << "\n";
    cout << "Brand: " << current->item.getBrand() << "\n";
    cout << "Quantity: " << current->quantity << "\n";

    current = current->prev;

    if (current == nullptr) {
        cout << "You have reached the beginning of the cart.\n";
    }
}

// Calculate the total price of all cart items.
double ShoppingCart::calculateTotal() const {

    double total = 0.0;

    Node* temp = head;

    while (temp != nullptr) {
        total += temp->item.getPrice() * temp->quantity;
        temp = temp->next;
    }

    return total;
}

// Check whether the cart is empty.
bool ShoppingCart::isEmpty() const {
    return head == nullptr;
}

// Return the number of different products in the cart.
int ShoppingCart::getSize() const {
    return size;
}


// Return independent copies of cart items for checkout.
vector<CartItem> ShoppingCart::getItems() const {

    vector<CartItem> items;

    items.reserve(size);

    Node* temp = head;

    while (temp != nullptr) {
        items.push_back({temp->item, temp->quantity});
        temp = temp->next;
    }

    return items;
}

// Remove all items after a successful checkout.
void ShoppingCart::clearCart() {

    Node* temp = head;

    while (temp != nullptr) {
        Node* nextNode = temp->next;
        delete temp;
        temp = nextNode;
    }

    head = nullptr;
    tail = nullptr;
    current = nullptr;
    size = 0;
}

