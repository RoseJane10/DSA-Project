
#pragma once

#include <string>
#include <vector>

class BrandShowcase {

private:

    // Circular Linked List Node.
    struct Node {
        std::string brandName;
        Node* next;

        Node(const std::string& name);
    };

    Node* head;
    Node* current;

    int size;

public:

    BrandShowcase();
    ~BrandShowcase();

    // Prevent unsafe copying of raw pointers.
    BrandShowcase(const BrandShowcase&) = delete;
    BrandShowcase& operator=(const BrandShowcase&) = delete;

    // Add a brand to the circular linked list.
    void addBrand(const std::string& brandName);

    // Display the current brand and move to the next.
    void displayNext();

    // Display all brands in the circular list.
    void displayAll() const;

    bool isEmpty() const;
    int getSize() const;
};
