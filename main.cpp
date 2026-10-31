
#include "Database.h"
#include "InventoryRepository.h"
#include "DSAComponents.h"
#include "ShoppingCart.h"
#include "BrandShowcase.h"
#include "InventoryUndoStack.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <limits>
#include <cmath>
#include <new>
#include <windows.h>
#include "AuthSystem.h"

HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

void setColor(int color) {
    SetConsoleTextAttribute(console, color);
}


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
         << product.getPrice();

    int stock = product.getStock();

    if (stock <= 5) {
        setColor(12); // Red
    }
    else if (stock <= 10) {
        setColor(14); // Yellow
    }
    else {
        setColor(10); // Green
    }

    cout << stock;

    setColor(7); // Reset to white

    cout << "\n";
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


// Lab 4: Stock management and undo using a LIFO stack.
void inventoryUndoMenu(
    InventoryRepository& repository,
    vector<Item>& products,
    InventoryUndoStack& undoStack
) {
    int choice = -1;

    do {
        cout << "\n============================================\n";
        cout << "       INVENTORY STOCK MANAGEMENT\n";
        cout << "             STACK (LIFO)\n";
        cout << "============================================\n";
        cout << "1. View Current Inventory\n";
        cout << "2. Update Product Stock\n";
        cout << "3. Undo Last Stock Change\n";
        cout << "4. View Latest Undo Action\n";
        cout << "5. View Undo Stack Size\n";
        cout << "0. Back to Main Menu\n";
        cout << "============================================\n";

        if (!readInteger("Enter your choice: ", choice)) {
            continue;
        }

        switch (choice) {
            case 1: {
                displayProducts(products);
                break;
            }

            case 2: {
                int productId;
                int newStock;

                cout << "\n========== UPDATE PRODUCT STOCK ==========\n";

                if (!readInteger("Enter Product ID: ", productId)) {
                    break;
                }

                Item* selectedProduct = nullptr;
                for (Item& product : products) {
                    if (product.getId() == productId) {
                        selectedProduct = &product;
                        break;
                    }
                }

                if (selectedProduct == nullptr) {
                    cout << "\nProduct ID not found.\n";
                    break;
                }

                const int previousStock = selectedProduct->getStock();

                cout << "\nProduct: " << selectedProduct->getName() << "\n";
                cout << "Current Stock: " << previousStock << "\n";

                if (!readInteger("Enter new stock quantity: ", newStock)) {
                    break;
                }

                if (newStock < 0) {
                    cout << "\nStock cannot be negative.\n";
                    break;
                }

                if (newStock == previousStock) {
                    cout << "\nThe new stock is the same as the current stock.\n";
                    break;
                }

                InventoryUndoStack::UndoAction action{
                    selectedProduct->getId(),
                    selectedProduct->getName(),
                    previousStock,
                    newStock
                };

                // Save the undo action before changing the database.
                try {
                    undoStack.push(action);
                } catch (const bad_alloc&) {
                    cout << "\nNot enough memory to save the undo action. "
                         << "Stock was not changed.\n";
                    break;
                }

                if (!repository.updateProductStock(
                        productId, previousStock, newStock)) {
                    InventoryUndoStack::UndoAction discarded;
                    undoStack.pop(discarded);
                    cout << "\nStock was not changed.\n";
                    break;
                }

                cout << "\nProduct stock successfully updated.\n";

                vector<Item> refreshedProducts = repository.getAllProducts();
                if (!refreshedProducts.empty()) {
                    products.swap(refreshedProducts);
                } else {
                    cout << "Warning: Stock was updated, but the catalog "
                         << "could not be refreshed. Restart or reload the "
                         << "application before relying on displayed stock.\n";
                }

                break;
            }

            case 3: {
                InventoryUndoStack::UndoAction action;

                cout << "\n========== UNDO LAST STOCK CHANGE ==========\n";

                if (!undoStack.peek(action)) {
                    cout << "\nNothing to undo. The undo stack is empty.\n";
                    break;
                }

                cout << "Product: " << action.productName << "\n";
                cout << "Previous Stock: " << action.previousStock << "\n";
                cout << "Updated Stock: " << action.updatedStock << "\n";

                int confirm;
                if (!readInteger("Undo this change? (1 = Yes, 0 = No): ",
                                 confirm)) {
                    break;
                }

                if (confirm != 1) {
                    cout << "\nUndo cancelled.\n";
                    break;
                }

                // Restore only if the database still contains the expected value.
                if (!repository.updateProductStock(
                        action.productId,
                        action.updatedStock,
                        action.previousStock)) {
                    cout << "\nUndo failed. The stack entry was kept. "
                         << "The product stock may have changed elsewhere.\n";
                    break;
                }

                InventoryUndoStack::UndoAction removedAction;
                undoStack.pop(removedAction);

                cout << "\nUndo successful. Previous stock restored.\n";

                vector<Item> refreshedProducts = repository.getAllProducts();
                if (!refreshedProducts.empty()) {
                    products.swap(refreshedProducts);
                } else {
                    cout << "Warning: Stock was restored, but the catalog "
                         << "could not be refreshed. Restart or reload the "
                         << "application before relying on displayed stock.\n";
                }

                break;
            }

            case 4:
                undoStack.displayLatest();
                break;

            case 5:
                cout << "\nUndo actions available: "
                     << undoStack.getSize() << "\n";
                break;

            case 0:
                cout << "\nReturning to BEAUTRIX main menu...\n";
                break;

            default:
                cout << "\nInvalid choice. Please try again.\n";
                break;
        }

    } while (choice != 0);
}

int main() {

    Database database;

    if (!database.connect()) {
        return 1;
    }
    
    // Authenticate user before opening the system.
AuthSystem authSystem(database);
AuthenticatedUser currentUser{0, "", ""};

bool authenticated = false;

for (int attempt = 1; attempt <= 3; attempt++) {

    if (authSystem.login(currentUser)) {
        authenticated = true;
        break;
    }

    if (attempt < 3) {
        cout << "\nLogin attempts remaining: "
             << (3 - attempt) << "\n";
    }
}

if (!authenticated) {
    cout << "\nMaximum login attempts reached.\n";
    cout << "Exiting BEAUTRIX...\n";
    return 0;
}

    InventoryRepository repository(database);

    vector<Item> products = repository.getAllProducts();

    if (products.empty()) {
        cout << "No products available or failed to retrieve inventory.\n";
        return 1;
    }

    // Create one shopping cart for this application session.
    ShoppingCart cart;

    // Lab 4: Stack of stock changes that can be undone.
    InventoryUndoStack undoStack;

    // Lab 3: Build a circular linked list of unique product brands.
    BrandShowcase showcase;
    for (const Item& product : products) {
        showcase.addBrand(product.getBrand());
    }

    int choice = -1;

    do {
        
        setColor(11);
        cout << "\n============================================\n";
        cout << "             BEAUTRIX SYSTEM\n";
        cout << "============================================\n";
        
        setColor(7);
       cout << "1. View Complete Catalog\n";
       cout << "2. Search Products by Keyword\n";
       cout << "3. Filter Products by Price Range\n";
       cout << "4. Sort Inventory\n";
       cout << "5. Shopping Cart (Doubly Linked List)\n";
       cout << "6. Partner Brand Showcase (Circular Linked List)\n";

      if (currentUser.role == "admin" ||
      currentUser.role == "staff") {

    cout << "7. Stock Management & Undo (Stack LIFO)\n";
}

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

            // Stack-based stock update and undo (Lab 4).
            case 7: {

            if (currentUser.role != "admin" &&
             currentUser.role != "staff") {

             cout << "\nAccess denied. Admin or staff account required.\n";
             break;
            }

    inventoryUndoMenu(repository, products, undoStack);

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
