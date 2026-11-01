
#include "Database.h"
#include "InventoryRepository.h"
#include "DSAComponents.h"
#include "ShoppingCart.h"
#include "BrandShowcase.h"
#include "InventoryUndoStack.h"
#include "AuthSystem.h"
#include "OrderQueue.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <limits>
#include <cmath>
#include <new>
#include <windows.h>
#include "ProductBST.h"

using namespace std;

HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

// Change console text color.
void setColor(int color) {
    SetConsoleTextAttribute(console, color);
}

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
            setColor(12); // Red: Low stock
        }
        else if (stock <= 10) {
            setColor(14); // Yellow: Medium stock
        }
        else {
            setColor(10); // Green: Sufficient stock
        }

        cout << stock;

        setColor(7);

        cout << "\n";
    }

    cout << "\nTotal Products: " << products.size() << "\n";
}

// ======================================================
// PRODUCT ID SEARCH
// Lab 6: Binary Search Tree
// ======================================================

void productBSTSearchMenu(
    ProductBST& productIndex,
    const vector<Item>& products
) {

    cout << "\n============================================\n";
    cout << "       PRODUCT ID SEARCH (BST)\n";
    cout << "============================================\n";

    try {

        // Rebuild index using the latest inventory.
        if (!productIndex.build(products)) {

            cout << "\nUnable to build the product BST.\n";
            cout << "Duplicate or invalid product IDs detected.\n";

            return;
        }

        cout << "Products indexed: "
             << productIndex.getSize() << "\n";

        int productId;

        if (!readInteger("Enter Product ID to search: ", productId)) {
            return;
        }

        const Item* result = productIndex.search(productId);

        if (result == nullptr) {

            cout << "\nProduct ID not found.\n";

            return;
        }

        cout << "\n<escape>========== PRODUCT FOUND ==========</escape>\n";

        vector<Item> foundProduct;
        foundProduct.push_back(*result);

        displayProducts(foundProduct);

    }
    catch (const bad_alloc&) {

        cout << "\nNot enough memory to build or search the BST.\n";
    }
}
// ======================================================
// SHOPPING CART MENU
// Lab 2: Doubly Linked List
// ======================================================

void shoppingCartMenu(
    ShoppingCart& cart,
    const vector<Item>& products,
    OrderQueue& orderQueue,
    const string& username
) {

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
        cout << "8. Submit Order to Checkout Queue\n";
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

                cout << "Price: P"
                     << fixed << setprecision(2)
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

                if (!readInteger(
                    "Enter Product ID to remove: ",
                    productId
                )) {
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

            // Submit cart to FIFO queue.
            case 8: {

                cout << "\n========== SUBMIT ORDER ==========\n";

                if (cart.isEmpty()) {
                    cout << "\nYour shopping cart is empty.\n";
                    break;
                }

                if (username.empty() || username.size() > 40) {
                    cout << "\nInvalid customer account information.\n";
                    break;
                }

                CustomerOrder order;
                order.username = username;

                try {

                    // Retrieve independent copies of cart items.
                    vector<CartItem> cartItems = cart.getItems();

                    if (cartItems.empty()) {
                        cout << "\nYour shopping cart is empty.\n";
                        break;
                    }

                    // Prepare the customer order.
                    order.items.reserve(cartItems.size());

                    for (const CartItem& item : cartItems) {

                        if (item.quantity <= 0 ||
                            item.item.getId() <= 0) {

                            cout << "\nInvalid cart item. "
                                 << "Order was not submitted.\n";

                            break;
                        }

                        order.items.push_back({
                            item.item,
                            item.quantity
                        });
                    }

                    if (order.items.size() != cartItems.size()) {
                        break;
                    }

                    // Add order to the queue.
                    orderQueue.enqueue(order);

                    // Clear cart after successful enqueue.
                    cart.clearCart();

                    cout << "\nYour order has been submitted!\n";

                    cout << "Queue Position: "
                         << orderQueue.getSize() << "\n";

                    cout << "Your order is waiting for checkout processing.\n";

                }
                catch (const bad_alloc&) {

                    cout << "\nNot enough memory to submit your order.\n";
                    cout << "Your shopping cart has been preserved.\n";
                }

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

// ======================================================
// CHECKOUT QUEUE MENU
// Lab 5: FIFO Queue
// ======================================================

void checkoutQueueMenu(
    OrderQueue& orderQueue,
    InventoryRepository& repository,
    vector<Item>& products,
    bool canProcess
) {

    int choice = -1;

    do {

        cout << "\n============================================\n";
        cout << "          CHECKOUT ORDER QUEUE\n";
        cout << "                FIFO\n";
        cout << "============================================\n";

        cout << "1. View Pending Orders\n";
        cout << "2. Process Next Order\n";
        cout << "0. Back to Main Menu\n";

        cout << "============================================\n";

        if (!readInteger("Enter your choice: ", choice)) {
            continue;
        }

        switch (choice) {

            // View pending orders.
            case 1: {

                orderQueue.displayQueue();

                break;
            }

            // Process the first order.
            case 2: {

                cout << "\n========== PROCESS NEXT ORDER ==========\n";

                if (!canProcess) {

                    cout << "\nAccess denied. "
                         << "Admin or staff account required.\n";

                    break;
                }

                CustomerOrder nextOrder;

                // Read the front order without removing it.
                try {

                    if (!orderQueue.peek(nextOrder)) {

                        cout << "\nNo pending orders to process.\n";

                        break;
                    }

                }
                catch (const bad_alloc&) {

                    cout << "\nNot enough memory to retrieve the order.\n";

                    break;
                }

                cout << "\nCustomer: "
                     << nextOrder.username << "\n";

                cout << "Products: "
                     << nextOrder.items.size() << "\n";

                int confirm;

                if (!readInteger(
                    "Process this order? (1 = Yes, 0 = No): ",
                    confirm
                )) {
                    break;
                }

                if (confirm != 1) {

                    cout << "\nOrder processing cancelled.\n";

                    break;
                }

                int orderId = 0;
                double orderTotal = 0.0;

                // Save order and deduct stock through PostgreSQL.
                if (!repository.processOrder(
                    nextOrder,
                    orderId,
                    orderTotal
                )) {

                    cout << "\nOrder was not completed.\n";
                    cout << "It remains in the checkout queue.\n";

                    break;
                }

                // Remove order only after successful database commit.
                if (!orderQueue.removeFront()) {

                    cout << "\nWarning: The database order was saved, "
                         << "but the queue could not remove its front item.\n";

                    break;
                }

                cout << "\n============================================\n";
                cout << "             ORDER COMPLETED\n";
                cout << "============================================\n";

                cout << "Order ID: " << orderId << "\n";
                cout << "Customer: " << nextOrder.username << "\n";

                cout << "Total Amount: P"
                     << fixed << setprecision(2)
                     << orderTotal << "\n";

                cout << "Remaining Pending Orders: "
                     << orderQueue.getSize() << "\n";

                cout << "============================================\n";

                // Refresh inventory after successful checkout.
                vector<Item> refreshedProducts =
                    repository.getAllProducts();

                if (!refreshedProducts.empty()) {

                    products.swap(refreshedProducts);

                }
                else {

                    cout << "\nWarning: Inventory could not be refreshed.\n";
                }

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

// ======================================================
// BRAND SHOWCASE MENU
// Lab 3: Circular Linked List
// ======================================================

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

// ======================================================
// INVENTORY STOCK MANAGEMENT
// Lab 4: Stack (LIFO)
// ======================================================

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

            // View inventory.
            case 1: {

                displayProducts(products);

                break;
            }

            // Update stock.
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

                cout << "\nProduct: "
                     << selectedProduct->getName() << "\n";

                cout << "Current Stock: "
                     << previousStock << "\n";

                if (!readInteger(
                    "Enter new stock quantity: ",
                    newStock
                )) {
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

                // Save undo action before changing the database.
                try {

                    undoStack.push(action);

                }
                catch (const bad_alloc&) {

                    cout << "\nNot enough memory to save the undo action. "
                         << "Stock was not changed.\n";

                    break;
                }

                if (!repository.updateProductStock(
                    productId,
                    previousStock,
                    newStock
                )) {

                    InventoryUndoStack::UndoAction discarded;

                    undoStack.pop(discarded);

                    cout << "\nStock was not changed.\n";

                    break;
                }

                cout << "\nProduct stock successfully updated.\n";

                vector<Item> refreshedProducts =
                    repository.getAllProducts();

                if (!refreshedProducts.empty()) {

                    products.swap(refreshedProducts);

                }
                else {

                    cout << "Warning: Stock was updated, but the catalog "
                         << "could not be refreshed.\n";
                }

                break;
            }

            // Undo last stock change.
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

                if (!readInteger(
                    "Undo this change? (1 = Yes, 0 = No): ",
                    confirm
                )) {
                    break;
                }

                if (confirm != 1) {

                    cout << "\nUndo cancelled.\n";

                    break;
                }

                if (!repository.updateProductStock(
                    action.productId,
                    action.updatedStock,
                    action.previousStock
                )) {

                    cout << "\nUndo failed. The stack entry was kept. "
                         << "The product stock may have changed elsewhere.\n";

                    break;
                }

                InventoryUndoStack::UndoAction removedAction;

                undoStack.pop(removedAction);

                cout << "\nUndo successful. Previous stock restored.\n";

                vector<Item> refreshedProducts =
                    repository.getAllProducts();

                if (!refreshedProducts.empty()) {

                    products.swap(refreshedProducts);

                }
                else {

                    cout << "Warning: Stock was restored, but the catalog "
                         << "could not be refreshed.\n";
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

// ======================================================
// MAIN FUNCTION
// ======================================================

int main() {

    // Connect to PostgreSQL.
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

    // Create inventory repository.
    InventoryRepository repository(database);

    // Retrieve products from PostgreSQL.
    vector<Item> products = repository.getAllProducts();

    if (products.empty()) {

        cout << "No products available or failed to retrieve inventory.\n";

        return 1;
    }

    // Lab 2: Shopping cart.
    ShoppingCart cart;

    // Lab 5: FIFO checkout queue.
    OrderQueue orderQueue;

    // Lab 4: LIFO undo stack.
    InventoryUndoStack undoStack;

    // Lab 3: Circular linked list brand showcase.
    BrandShowcase showcase;
    
    //Lab 6
    ProductBST productIndex;

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

        cout << "Logged in as: " << currentUser.username << "\n";
        cout << "Account Role: " << currentUser.role << "\n";

        cout << "============================================\n";

        cout << "1. View Complete Catalog\n";
        cout << "2. Search Products by Keyword\n";
        cout << "3. Filter Products by Price Range\n";
        cout << "4. Sort Inventory\n";
        cout << "5. Shopping Cart\n";
        cout << "6. Partner Brand Showcase\n";
        cout << "7. Checkout Order Queue\n";

        if (currentUser.role == "admin" ||
            currentUser.role == "staff") {

            cout << "8. Stock Management & Undo\n";
        }

        cout << "9. Search Product by ID\n";
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

                if (keyword.find_first_not_of(" \t\n\r") ==
                    string::npos) {

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

                if (!isfinite(minPrice) ||
                    !isfinite(maxPrice) ||
                    minPrice < 0 ||
                    maxPrice < 0) {

                    cout << "\nPrices cannot be negative or infinite.\n";

                    break;
                }

                if (minPrice > maxPrice) {

                    cout << "\nMinimum price cannot be greater than maximum price.\n";

                    break;
                }

                vector<Item> results =
                    binarySearchPriceRange(
                        products,
                        minPrice,
                        maxPrice
                    );

                cout << "\n========== PRICE RANGE RESULTS ==========\n";

                cout << "Price range: "
                     << fixed << setprecision(2)
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

                    case 1: {

                        sortedProducts =
                            insertionSortByBrand(products);

                        cout << "\n========== SORTED BY BRAND ==========\n";

                        displayProducts(sortedProducts);

                        break;
                    }

                    case 2: {

                        sortedProducts =
                            quickSortByPrice(products);

                        cout << "\n========== SORTED BY PRICE ==========\n";

                        displayProducts(sortedProducts);

                        break;
                    }

                    case 3: {

                        sortedProducts =
                            quickSortByStock(products);

                        cout << "\n========== SORTED BY STOCK ==========\n";

                        displayProducts(sortedProducts);

                        break;
                    }

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

                shoppingCartMenu(
                    cart,
                    products,
                    orderQueue,
                    currentUser.username
                );

                break;
            }

            // Brand showcase.
            case 6: {

                brandShowcaseMenu(showcase);

                break;
            }

            // Lab 5: FIFO checkout queue.
            case 7: {

                bool canProcess =
                    currentUser.role == "admin" ||
                    currentUser.role == "staff";

                checkoutQueueMenu(
                    orderQueue,
                    repository,
                    products,
                    canProcess
                );

                break;
            }

            // Lab 4: Stack-based stock management.
            case 8: {

                if (currentUser.role != "admin" &&
                    currentUser.role != "staff") {

                    cout << "\nAccess denied. "
                         << "Admin or staff account required.\n";

                    break;
                }

                inventoryUndoMenu(
                    repository,
                    products,
                    undoStack
                );

                break;
            }

            case 9: {

    productBSTSearchMenu(productIndex, products);

    break;
}

            case 0: {

                cout << "\nThank you for using BEAUTRIX!\n";

                break;
            }

            default: {

                cout << "\nInvalid choice. Please try again.\n";

                break;
            }
        }

    } while (choice != 0);

    return 0;
}
