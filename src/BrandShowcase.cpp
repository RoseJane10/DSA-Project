
#include "BrandShowcase.h"

#include <iostream>

using namespace std;

// Create a new circular linked list node.
BrandShowcase::Node::Node(const string& name)
    : brandName(name), next(nullptr) {
}

// Initialize an empty showcase.
BrandShowcase::BrandShowcase()
    : head(nullptr), current(nullptr), size(0) {
}

// Free all dynamically allocated nodes.
BrandShowcase::~BrandShowcase() {

    if (head == nullptr) {
        return;
    }

    Node* temp = head->next;

    // Stop when we reach the first node again.
    while (temp != head) {
        Node* nextNode = temp->next;
        delete temp;
        temp = nextNode;
    }

    delete head;

    head = nullptr;
    current = nullptr;
    size = 0;
}

// Add a brand to the circular linked list.
void BrandShowcase::addBrand(const string& brandName) {

    if (brandName.empty()) {
        cout << "Brand name cannot be empty.\n";
        return;
    }

    // Check for duplicate brands.
    if (head != nullptr) {

        Node* temp = head;

        do {
            if (temp->brandName == brandName) {
                return;
            }

            temp = temp->next;

        } while (temp != head);
    }

    Node* newNode = new Node(brandName);

    // First node points to itself.
    if (head == nullptr) {

        head = newNode;
        newNode->next = head;
        current = head;

    }
    else {

        // Find the last node.
        Node* tail = head;

        while (tail->next != head) {
            tail = tail->next;
        }

        // Connect the new node.
        tail->next = newNode;
        newNode->next = head;
    }

    size++;
}

// Display the current brand and move forward.
void BrandShowcase::displayNext() {

    if (isEmpty()) {
        cout << "No partner brands available.\n";
        return;
    }

    cout << "\n========== PARTNER BRAND SHOWCASE ==========\n";

    cout << "Current Brand: " << current->brandName << "\n";

    current = current->next;

    cout << "Next Brand: " << current->brandName << "\n";
}

// Display every brand in the circular linked list.
void BrandShowcase::displayAll() const {

    if (isEmpty()) {
        cout << "No partner brands available.\n";
        return;
    }

    cout << "\n========== ALL PARTNER BRANDS ==========\n";

    Node* temp = head;
    int number = 1;

    do {

        cout << number << ". " << temp->brandName << "\n";

        temp = temp->next;
        number++;

    } while (temp != head);

    cout << "\nTotal Partner Brands: " << size << "\n";

    cout << "Last brand connects back to: "
         << head->brandName << "\n";
}

// Check whether the list is empty.
bool BrandShowcase::isEmpty() const {
    return head == nullptr;
}

// Return the number of unique brands.
int BrandShowcase::getSize() const {
    return size;
}
