
#include "Database.h"
#include "InventoryRepository.h"
#include "DSAComponents.h"
#include "ShoppingCart.h"
#include "BrandShowcase.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <limits>
#include <cmath>

using namespace std;

// Read an integer safely.
bool readInteger(const string& prompt, int& value) {

    cout << prompt;

    if (!(cin >> value)) {
        cout << "\nInvalid input. Please enter a valid number.\n";

        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        return false;
    }

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    return true;
}

// Display products in a table.
void displayProducts(const vector<Item>& products) {

    if (products.empty()) {
        cout << "\nNo products found.\n";
        return;
    }

    cout << "\n"
         << left
         << setw(5) << "ID"
         << setw(30) << "PRODUCT"
         << setw(25) << "BRAND"
         << setw(15) << "CATEGORY"
         << setw(12) << "PRICE"
         << "STOCK\n";

    cout << string(95, '-') << "\n";

    for (const Item& product : products) {

        cout << left
             << setw(5) << product.getId()
             << setw(30) << product.getName().substr(0, 28)
             << setw(25) << product.getBrand().substr(0, 23)
             << setw(15) << product.getCategory().substr(0, 13)
             << setw(12) << fixed << setprecision(2)
             << product.getPrice()
             << product.getStock()
             << "\n";
    }

    cout << "\nTotal Products: " << products.size() << "\n";
}

// Shopping cart submenu.
void shoppingCartMenu(ShoppingCart& cart, const vector<Item>& products) {

    int choice = -1;

    do {

        cout << "\n============================================\n";
        cout << "              SHOPPING CART\n";
        cout << "============================================\n";

        cout << "1. Add Item to Cart\n";
        cout << "2. View Shopping Cart\n";
        cout << "3. Remove Item from Cart\n";
        cout << "4. Update Item Quantity\n";
        cout << "5. Navigate Forward\n";
        cout << "6. Navigate Backward\n";
        cout << "7. Display Cart Total\n";
        cout << "0. Back to Main Menu\n";

        cout << "============================================\n";

        if (!readInteger("Enter your choice: ", choice)) {
            continue;
        }

        switch (choice) {

            // Add item.
            case 1: {

                int productId;
                int quantity;

                cout << "\n========== ADD ITEM TO CART ==========\n";

                if (!readInteger("Enter Product ID: ", productId)) {
                    break;
                }

                // Find the product in the inventory.
                const Item* selectedItem = nullptr;

                for (const Item& product : products) {

                    if (product.getId() == productId) {
                        selectedItem = &product;
                        break;
                    }
                }

                if (selectedItem == nullptr) {
                    cout << "\nProduct ID not found in inventory.\n";
                    break;
                }

                cout << "\nProduct: " << selectedItem->getName() << "\n";
                cout << "Brand: " << selectedItem->getBrand() << "\n";
                cout << "Price: P" << fixed << setprecision(2)
                     << selectedItem->getPrice() << "\n";
                cout << "Available Stock: "
                     << selectedItem->getStock() << "\n";

                if (!readInteger("Enter quantity: ", quantity)) {
                    break;
                }

                cart.addItem(*selectedItem, quantity);

                break;
            }

            // View cart.
            case 2: {

                cout << "\n========== VIEW SHOPPING CART ==========\n";

                cart.displayCart();

                break;
            }

            // Remove item.
            case 3: {

                int productId;

                cout << "\n========== REMOVE CART ITEM ==========\n";

                if (!readInteger("Enter Product ID to remove: ", productId)) {
                    break;
                }

                cart.removeItem(productId);

                break;
            }

            // Update quantity.
            case 4: {

                int productId;
                int quantity;

                cout << "\n========== UPDATE CART QUANTITY ==========\n";

                if (!readInteger("Enter Product ID: ", productId)) {
                    break;
                }

                if (!readInteger("Enter new quantity: ", quantity)) {
                    break;
                }

                cart.updateQuantity(productId, quantity);

                break;
            }

            // Navigate forward.
            case 5: {

                cout << "\n========== FORWARD NAVIGATION ==========\n";

                cart.displayForward();

                break;
            }

            // Navigate backward.
            case 6: {

                cout << "\n========== BACKWARD NAVIGATION ==========\n";

                cart.displayBackward();

                break;
            }

            // Display total.
            case 7: {

                cout << "\n========== CART TOTAL ==========\n";

                cout << "Total Products: " << cart.getSize() << "\n";

                cout << "Grand Total: P"
                     << fixed << setprecision(2)
                     << cart.calculateTotal() << "\n";

                break;
            }

            case 0:

                cout << "\nReturning to BEAUTRIX main menu...\n";

                break;

            default:

                cout << "\nInvalid choice. Please try again.\n";
        }

    } while (choice != 0);
}


// Partner brand showcase submenu (Lab 3: Circular Linked List).
void brandShowcaseMenu(BrandShowcase& showcase) {
    int choice = -1;

    do {
        cout << "\n============================================\n";
        cout << "          PARTNER BRAND SHOWCASE\n";
        cout << "============================================\n";
        cout << "1. Display All Partner Brands\n";
        cout << "2. Show Next Brand (Circular Cycle)\n";
        cout << "0. Back to Main Menu\n";
        cout << "============================================\n";

        if (!readInteger("Enter your choice: ", choice)) {
            continue;
        }

        switch (choice) {
            case 1:
                showcase.displayAll();
                break;

            case 2:
                showcase.displayNext();
                break;

            case 0:
                cout << "\nReturning to BEAUTRIX main menu...\n";
                break;

            default:
                cout << "\nInvalid choice. Please try again.\n";
        }

    } while (choice != 0);
}

int main() {

    Database database;

    if (!database.connect()) {
        return 1;
    }

    InventoryRepository repository(database);

    vector<Item> products = repository.getAllProducts();

    if (products.empty()) {
        cout << "No products available or failed to retrieve inventory.\n";
        return 1;
    }

    // Create one shopping cart for this application session.
    ShoppingCart cart;

    // Lab 3: Build a circular linked list of unique product brands.
    BrandShowcase showcase;
    for (const Item& product : products) {
        showcase.addBrand(product.getBrand());
    }

    int choice = -1;

    do {

        cout << "\n============================================\n";
        cout << "             BEAUTRIX SYSTEM\n";
        cout << "============================================\n";

        cout << "1. View Complete Catalog\n";
        cout << "2. Search Products by Keyword\n";
        cout << "3. Filter Products by Price Range\n";
        cout << "4. Sort Inventory\n";
        cout << "5. Shopping Cart (Doubly Linked List)\n";
        cout << "6. Partner Brand Showcase (Circular Linked List)\n";
        cout << "0. Exit\n";

        cout << "============================================\n";

        if (!readInteger("Enter your choice: ", choice)) {
            continue;
        }

        switch (choice) {

            // Complete catalog.
            case 1: {

                cout << "\n========== COMPLETE PRODUCT CATALOG ==========\n";

                displayProducts(products);

                break;
            }

            // Linear search.
            case 2: {

                string keyword;

                cout << "\n========== LINEAR SEARCH ==========\n";

                cout << "Enter product name or brand: ";
                getline(cin, keyword);

                if (keyword.find_first_not_of(" \t\n\r") == string::npos) {
                    cout << "\nSearch keyword cannot be empty.\n";
                    break;
                }

                vector<Item> results =
                    linearSearchProducts(products, keyword);

                cout << "\n========== SEARCH RESULTS ==========\n";

                displayProducts(results);

                break;
            }

            // Binary search price filter.
            case 3: {

                double minPrice;
                double maxPrice;

                cout << "\n========== BINARY SEARCH: PRICE FILTER ==========\n";

                cout << "Enter minimum price: ";

                if (!(cin >> minPrice)) {
                    cout << "\nInvalid input. Please enter a valid number.\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    break;
                }

                cout << "Enter maximum price: ";

                if (!(cin >> maxPrice)) {
                    cout << "\nInvalid input. Please enter a valid number.\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    break;
                }

                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                if (!isfinite(minPrice) || !isfinite(maxPrice) ||
                    minPrice < 0 || maxPrice < 0) {

                    cout << "\nPrices cannot be negative or infinite.\n";
                    break;
                }

                if (minPrice > maxPrice) {
                    cout << "\nMinimum price cannot be greater than maximum price.\n";
                    break;
                }

                vector<Item> results =
                    binarySearchPriceRange(products, minPrice, maxPrice);

                cout << "\n========== PRICE RANGE RESULTS ==========\n";

                cout << "Price range: " << fixed << setprecision(2)
                     << minPrice << " - " << maxPrice << "\n";

                displayProducts(results);

                break;
            }

            // Inventory sorting.
            case 4: {

                int sortChoice;

                cout << "\n========== INVENTORY SORTING ==========\n";

                cout << "1. Sort by Brand (Insertion Sort)\n";
                cout << "2. Sort by Price (Quick Sort)\n";
                cout << "3. Sort by Stock (Quick Sort)\n";
                cout << "0. Back to Main Menu\n";

                if (!readInteger("Enter your choice: ", sortChoice)) {
                    break;
                }

                vector<Item> sortedProducts;

                switch (sortChoice) {

                    case 1:

                        sortedProducts = insertionSortByBrand(products);

                        cout << "\n========== SORTED BY BRAND ==========\n";

                        displayProducts(sortedProducts);

                        break;

                    case 2:

                        sortedProducts = quickSortByPrice(products);

                        cout << "\n========== SORTED BY PRICE ==========\n";

                        displayProducts(sortedProducts);

                        break;

                    case 3:

                        sortedProducts = quickSortByStock(products);

                        cout << "\n========== SORTED BY STOCK ==========\n";

                        displayProducts(sortedProducts);

                        break;

                    case 0:

                        cout << "\nReturning to main menu...\n";

                        break;

                    default:

                        cout << "\nInvalid sorting option.\n";
                }

                break;
            }

            // Shopping cart.
            case 5: {

                shoppingCartMenu(cart, products);

                break;
            }

            // Circular linked list brand showcase (Lab 3).
            case 6: {

                brandShowcaseMenu(showcase);

                break;
            }

            case 0:

                cout << "\nThank you for using BEAUTRIX!\n";

                break;

            default:

                cout << "\nInvalid choice. Please try again.\n";
        }

    } while (choice != 0);

    return 0;
}
