
#include "Database.h"
#include "InventoryRepository.h"
#include "DSAComponents.h"
#include "ShoppingCart.h"
#include "BrandShowcase.h"
#include "InventoryUndoStack.h"
#include "AuthSystem.h"
#include "OrderQueue.h"
#include "ProductBST.h"
#include "DynamicProductCatalog.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <limits>
#include <cmath>
#include <new>
#include <cstdlib>
#include <algorithm>
#include <cctype>
#include <windows.h>

using namespace std;

HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

// ======================================================
// CONSOLE DESIGN AND DISPLAY FUNCTIONS
// ======================================================

void setColor(int color) {
    SetConsoleTextAttribute(console, color);
}

void clearScreen() {
    system("cls");
}

void printCentered(const string& text, int color = 7) {

    CONSOLE_SCREEN_BUFFER_INFO info;
    int consoleWidth = 80;

    if (GetConsoleScreenBufferInfo(console, &info)) {
        consoleWidth = info.srWindow.Right -
                       info.srWindow.Left + 1;
    }

    int padding = (consoleWidth -
                   static_cast<int>(text.length())) / 2;

    if (padding < 0) {
        padding = 0;
    }

    setColor(color);
    cout << string(padding, ' ') << text << '\n';
    setColor(7);
}

void printMenuOption(
    int number,
    const string& option,
    int color = 7
) {

    CONSOLE_SCREEN_BUFFER_INFO info;
    int consoleWidth = 80;

    if (GetConsoleScreenBufferInfo(console, &info)) {
        consoleWidth = info.srWindow.Right -
                       info.srWindow.Left + 1;
    }

    const int menuWidth = 48;

    int indent = (consoleWidth - menuWidth) / 2;

    if (indent < 0) {
        indent = 0;
    }

    cout << string(indent, ' ');

    setColor(color);

    cout << right << setw(5)
         << (to_string(number) + ".");

    setColor(7);

    cout << left << option << '\n';
}

void displayHeader(
    const string& title,
    const string& subtitle = ""
) {

    cout << '\n';

    printCentered("BEAUTRIX", 13);
    printCentered("BEAUTY COSMETICS INVENTORY SYSTEM", 11);

    cout << '\n';

    printCentered(title, 14);

    if (!subtitle.empty()) {
        printCentered(subtitle, 8);
    }

    cout << '\n';
}

void pauseForUser() {
    cout << '\n';
    printCentered("Press ENTER to continue...", 8);
    cin.get();
}

bool readInteger(const string& prompt, int& value) {

    cout << prompt;

    if (!(cin >> value)) {

        cout << "\nInvalid input. Please enter a valid number.\n";

        cin.clear();

        cin.ignore(
            numeric_limits<streamsize>::max(),
            '\n'
        );

        return false;
    }

    cin.ignore(
        numeric_limits<streamsize>::max(),
        '\n'
    );

    return true;
}

// ======================================================
// OFFICIAL BRAND FILTER HELPERS
// ======================================================

// Convert text to lowercase for case-insensitive comparison.
string normalizeBrandName(const string& text) {

    string result = text;

    transform(
        result.begin(),
        result.end(),
        result.begin(),
        [](unsigned char character) {
            return static_cast<char>(tolower(character));
        }
    );

    return result;
}

// Display unique brands available in the inventory.
void displayAvailableBrands(const vector<Item>& products) {

    vector<string> brands;

    for (const Item& product : products) {

        string brand = product.getBrand();

        bool alreadyExists = false;

        for (const string& existingBrand : brands) {

            if (normalizeBrandName(existingBrand) ==
                normalizeBrandName(brand)) {

                alreadyExists = true;
                break;
            }
        }

        if (!alreadyExists) {
            brands.push_back(brand);
        }
    }

    sort(
        brands.begin(),
        brands.end(),
        [](const string& a, const string& b) {
            return normalizeBrandName(a) <
                   normalizeBrandName(b);
        }
    );

    cout << "\nAVAILABLE OFFICIAL BRANDS\n\n";

    for (size_t i = 0; i < brands.size(); i++) {

        cout << setw(3) << (i + 1)
             << ". " << brands[i] << '\n';
    }

    cout << "\nTotal Available Brands: "
         << brands.size() << '\n';
}

// ======================================================
// PRODUCT DISPLAY
// ======================================================

void displayProducts(const vector<Item>& products) {

    if (products.empty()) {
        cout << "\nNo products found.\n";
        return;
    }

    cout << '\n';

    setColor(11);

    cout << left
         << setw(5) << "ID"
         << setw(30) << "PRODUCT"
         << setw(25) << "BRAND"
         << setw(15) << "CATEGORY"
         << setw(12) << "PRICE"
         << "STOCK\n";

    setColor(8);
    cout << string(95, '-') << "\n";
    setColor(7);

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
            setColor(12);
        }
        else if (stock <= 10) {
            setColor(14);
        }
        else {
            setColor(10);
        }

        cout << stock;

        setColor(7);
        cout << '\n';
    }

    cout << '\n';

    printCentered(
        "Total Products: " + to_string(products.size()),
        11
    );
}

// ======================================================
// LAB 1: DYNAMIC PRODUCT CATALOG
// ======================================================

void displayDynamicCatalog(
    const DynamicProductCatalog& catalog
) {

    displayHeader(
        "PRODUCT CATALOG",
        "Available beauty products"
    );

    printCentered(
        "Products Stored: " + to_string(catalog.getSize()),
        10
    );

    printCentered(
        "Allocated Capacity: " + to_string(catalog.getCapacity()),
        11
    );

    vector<Item> products = catalog.toVector();

    displayProducts(products);
}

// ======================================================
// LAB 6: PRODUCT ID SEARCH - BINARY SEARCH TREE
// ======================================================

void productBSTSearchMenu(
    ProductBST& productIndex,
    const vector<Item>& products
) {

    clearScreen();

    displayHeader(
        "PRODUCT ID SEARCH",
        "Binary Search Tree"
    );

    try {

        if (!productIndex.build(products)) {

            cout << "Unable to build the product BST.\n";
            cout << "Duplicate or invalid product IDs detected.\n";

            return;
        }

        cout << "Products indexed: "
             << productIndex.getSize() << "\n\n";

        int productId;

        if (!readInteger(
            "Enter Product ID to search: ",
            productId
        )) {
            return;
        }

        const Item* result =
            productIndex.search(productId);

        if (result == nullptr) {
            cout << "\nProduct ID not found.\n";
            return;
        }

        setColor(10);
        cout << "\nPRODUCT FOUND\n";
        setColor(7);

        vector<Item> foundProduct;
        foundProduct.push_back(*result);

        displayProducts(foundProduct);
    }
    catch (const bad_alloc&) {

        cout << "\nNot enough memory to build or search the BST.\n";
    }
}

// ======================================================
// LAB 2: SHOPPING CART - DOUBLY LINKED LIST
// ======================================================

void shoppingCartMenu(
    ShoppingCart& cart,
    const vector<Item>& products,
    OrderQueue& orderQueue,
    const string& username
) {

    int choice = -1;

    do {

        clearScreen();

        displayHeader(
            "SHOPPING CART",
            "Manage your selected products"
        );

        printMenuOption(1, "Add Item to Cart");
        printMenuOption(2, "View Shopping Cart");
        printMenuOption(3, "Remove Item from Cart");
        printMenuOption(4, "Update Item Quantity");
        printMenuOption(5, "Navigate Forward");
        printMenuOption(6, "Navigate Backward");
        printMenuOption(7, "Display Cart Total");
        printMenuOption(8, "Submit Order to Checkout Queue");

        cout << '\n';
        printMenuOption(0, "Back to Main Menu", 12);
        cout << '\n';

        if (!readInteger("Enter your choice: ", choice)) {
            continue;
        }

        switch (choice) {

            case 1: {

                int productId;
                int quantity;

                cout << "\nADD ITEM TO CART\n\n";

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

                cout << "\nProduct: "
                     << selectedItem->getName() << '\n';

                cout << "Brand: "
                     << selectedItem->getBrand() << '\n';

                cout << "Price: P"
                     << fixed << setprecision(2)
                     << selectedItem->getPrice() << '\n';

                cout << "Available Stock: "
                     << selectedItem->getStock() << '\n';

                if (!readInteger("Enter quantity: ", quantity)) {
                    break;
                }

                cart.addItem(*selectedItem, quantity);

                break;
            }

            case 2:

                cout << "\nSHOPPING CART\n";
                cart.displayCart();

                break;

            case 3: {

                int productId;

                cout << "\nREMOVE CART ITEM\n\n";

                if (!readInteger(
                    "Enter Product ID to remove: ",
                    productId
                )) {
                    break;
                }

                cart.removeItem(productId);

                break;
            }

            case 4: {

                int productId;
                int quantity;

                cout << "\nUPDATE CART QUANTITY\n\n";

                if (!readInteger("Enter Product ID: ", productId)) {
                    break;
                }

                if (!readInteger("Enter new quantity: ", quantity)) {
                    break;
                }

                cart.updateQuantity(productId, quantity);

                break;
            }

            case 5:

                cout << "\nFORWARD NAVIGATION\n";
                cart.displayForward();

                break;

            case 6:

                cout << "\nBACKWARD NAVIGATION\n";
                cart.displayBackward();

                break;

            case 7:

                cout << "\nCART SUMMARY\n\n";

                cout << "Total Products: "
                     << cart.getSize() << '\n';

                cout << "Grand Total: P"
                     << fixed << setprecision(2)
                     << cart.calculateTotal() << '\n';

                break;

            case 8: {

                cout << "\nSUBMIT ORDER\n\n";

                if (cart.isEmpty()) {
                    cout << "Your shopping cart is empty.\n";
                    break;
                }

                if (username.empty() || username.size() > 40) {
                    cout << "Invalid customer account information.\n";
                    break;
                }

                CustomerOrder order;
                order.username = username;

                try {

                    vector<CartItem> cartItems = cart.getItems();

                    if (cartItems.empty()) {
                        cout << "Your shopping cart is empty.\n";
                        break;
                    }

                    order.items.reserve(cartItems.size());

                    bool validOrder = true;

                    for (const CartItem& item : cartItems) {

                        if (item.quantity <= 0 ||
                            item.item.getId() <= 0) {

                            cout << "\nInvalid cart item. "
                                 << "Order was not submitted.\n";

                            validOrder = false;
                            break;
                        }

                        order.items.push_back({
                            item.item,
                            item.quantity
                        });
                    }

                    if (!validOrder) {
                        break;
                    }

                    orderQueue.enqueue(order);
                    cart.clearCart();

                    setColor(10);
                    cout << "\nYour order has been submitted!\n";
                    setColor(7);

                    cout << "Queue Position: "
                         << orderQueue.getSize() << '\n';

                    cout << "Your order is waiting for checkout processing.\n";
                }
                catch (const bad_alloc&) {

                    cout << "\nNot enough memory to submit your order.\n";
                    cout << "Your shopping cart has been preserved.\n";
                }

                break;
            }

            case 0:
                break;

            default:
                cout << "\nInvalid choice. Please try again.\n";
        }

        if (choice != 0) {
            pauseForUser();
        }

    } while (choice != 0);
}

// ======================================================
// LAB 5: CHECKOUT QUEUE - FIFO
// ======================================================

void checkoutQueueMenu(
    OrderQueue& orderQueue,
    InventoryRepository& repository,
    vector<Item>& products,
    bool canProcess
) {

    int choice = -1;

    do {

        clearScreen();

        displayHeader(
            "CHECKOUT ORDER QUEUE",
            "First In, First Out (FIFO)"
        );

        printMenuOption(1, "View Pending Orders");
        printMenuOption(2, "Process Next Order");

        cout << '\n';
        printMenuOption(0, "Back to Main Menu", 12);
        cout << '\n';

        if (!readInteger("Enter your choice: ", choice)) {
            continue;
        }

        switch (choice) {

            case 1:

                cout << "\nPENDING ORDERS\n";
                orderQueue.displayQueue();

                break;

            case 2: {

                cout << "\nPROCESS NEXT ORDER\n\n";

                if (!canProcess) {

                    cout << "Access denied. "
                         << "Admin or staff account required.\n";

                    break;
                }

                CustomerOrder nextOrder;

                try {

                    if (!orderQueue.peek(nextOrder)) {
                        cout << "No pending orders to process.\n";
                        break;
                    }
                }
                catch (const bad_alloc&) {

                    cout << "Not enough memory to retrieve the order.\n";
                    break;
                }

                cout << "Customer: "
                     << nextOrder.username << '\n';

                cout << "Products: "
                     << nextOrder.items.size() << '\n';

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

                if (!repository.processOrder(
                    nextOrder,
                    orderId,
                    orderTotal
                )) {

                    cout << "\nOrder was not completed.\n";
                    cout << "It remains in the checkout queue.\n";
                    break;
                }

                if (!orderQueue.removeFront()) {

                    cout << "\nWarning: The database order was saved, "
                         << "but the queue could not remove its front item.\n";

                    break;
                }

                setColor(10);
                cout << "\nORDER COMPLETED\n";
                setColor(7);

                cout << "Order ID: " << orderId << '\n';
                cout << "Customer: " << nextOrder.username << '\n';

                cout << "Total Amount: P"
                     << fixed << setprecision(2)
                     << orderTotal << '\n';

                cout << "Remaining Pending Orders: "
                     << orderQueue.getSize() << '\n';

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
                break;

            default:
                cout << "\nInvalid choice. Please try again.\n";
        }

        if (choice != 0) {
            pauseForUser();
        }

    } while (choice != 0);
}

// ======================================================
// LAB 3: BRAND SHOWCASE - CIRCULAR LINKED LIST
// ======================================================

void brandShowcaseMenu(BrandShowcase& showcase) {

    int choice = -1;

    do {

        clearScreen();

        displayHeader(
            "PARTNER BRAND SHOWCASE",
            "Explore our partner brands"
        );

        printMenuOption(1, "Display All Partner Brands");
        printMenuOption(2, "Show Next Brand (Circular Cycle)");

        cout << '\n';
        printMenuOption(0, "Back to Main Menu", 12);
        cout << '\n';

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
                break;

            default:
                cout << "\nInvalid choice. Please try again.\n";
        }

        if (choice != 0) {
            pauseForUser();
        }

    } while (choice != 0);
}

// ======================================================
// LAB 4: INVENTORY STOCK MANAGEMENT - STACK (LIFO)
// ======================================================

void inventoryUndoMenu(
    InventoryRepository& repository,
    vector<Item>& products,
    InventoryUndoStack& undoStack
) {

    int choice = -1;

    do {

        clearScreen();

        displayHeader(
            "STOCK MANAGEMENT",
            "Inventory updates and undo history"
        );

        printMenuOption(1, "View Current Inventory");
        printMenuOption(2, "Update Product Stock");
        printMenuOption(3, "Undo Last Stock Change");
        printMenuOption(4, "View Latest Undo Action");
        printMenuOption(5, "View Undo Stack Size");

        cout << '\n';
        printMenuOption(0, "Back to Main Menu", 12);
        cout << '\n';

        if (!readInteger("Enter your choice: ", choice)) {
            continue;
        }

        switch (choice) {

            case 1:

                displayProducts(products);

                break;

            case 2: {

                int productId;
                int newStock;

                cout << "\nUPDATE PRODUCT STOCK\n\n";

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
                     << selectedProduct->getName() << '\n';

                cout << "Current Stock: "
                     << previousStock << '\n';

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
                    cout << "\nThe new stock is the same as "
                         << "the current stock.\n";
                    break;
                }

                InventoryUndoStack::UndoAction action{
                    selectedProduct->getId(),
                    selectedProduct->getName(),
                    previousStock,
                    newStock
                };

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

                setColor(10);
                cout << "\nProduct stock successfully updated.\n";
                setColor(7);

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

            case 3: {

                InventoryUndoStack::UndoAction action;

                cout << "\nUNDO LAST STOCK CHANGE\n\n";

                if (!undoStack.peek(action)) {
                    cout << "Nothing to undo. The undo stack is empty.\n";
                    break;
                }

                cout << "Product: " << action.productName << '\n';
                cout << "Previous Stock: " << action.previousStock << '\n';
                cout << "Updated Stock: " << action.updatedStock << '\n';

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

                setColor(10);
                cout << "\nUndo successful. Previous stock restored.\n";
                setColor(7);

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
                     << undoStack.getSize() << '\n';

                break;

            case 0:
                break;

            default:
                cout << "\nInvalid choice. Please try again.\n";
                break;
        }

        if (choice != 0) {
            pauseForUser();
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

    // Authenticate user.
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
                 << (3 - attempt) << '\n';
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

    // Lab 1: Dynamic Product Catalog.
    DynamicProductCatalog dynamicCatalog;

    try {
        dynamicCatalog.load(products);
    }
    catch (const bad_alloc&) {

        cout << "\nUnable to allocate memory for the product catalog.\n";
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

    // Lab 6: Binary search tree.
    ProductBST productIndex;

    for (const Item& product : products) {
        showcase.addBrand(product.getBrand());
    }

    int choice = -1;

    do {

        clearScreen();

        displayHeader(
            "MAIN MENU",
            "Welcome, " + currentUser.username +
            " | Role: " + currentUser.role
        );

        printCentered("INVENTORY MANAGEMENT", 13);

                cout << '\n';

        printMenuOption(1, "View Complete Catalog");
        printMenuOption(2, "Search Products by Keyword");
        printMenuOption(3, "Filter Products by Price Range");
        printMenuOption(4, "Sort Inventory");
        printMenuOption(5, "Shopping Cart");
        printMenuOption(6, "Partner Brand Showcase");
        printMenuOption(7, "Checkout Order Queue");

        if (currentUser.role == "admin" ||
            currentUser.role == "staff") {

            printMenuOption(8, "Stock Management & Undo");
        }
        else {
            printMenuOption(8, "Stock Management & Undo [RESTRICTED]", 8);
        }

        printMenuOption(9, "Search Product by ID");
        printMenuOption(10, "Filter Products by Official Brand");

        cout << '\n';
        printMenuOption(0, "Exit BEAUTRIX", 12);
        cout << '\n';

        if (!readInteger("Enter your choice: ", choice)) {
            continue;
        }

        switch (choice) {

            // Lab 1: Dynamic product catalog.
            case 1: {

                clearScreen();
                displayDynamicCatalog(dynamicCatalog);

                break;
            }

            // Lab 7: Linear search.
            case 2: {

                clearScreen();

                displayHeader(
                    "PRODUCT SEARCH",
                    "Linear Search"
                );

                string keyword;

                cout << "Enter product name or brand: ";
                getline(cin, keyword);

                if (keyword.find_first_not_of(" \t\n\r") ==
                    string::npos) {

                    cout << "\nSearch keyword cannot be empty.\n";
                    break;
                }

                vector<Item> results =
                    linearSearchProducts(products, keyword);

                cout << "\nSEARCH RESULTS\n";
                displayProducts(results);

                break;
            }

            // Lab 7: Binary search price filter.
            case 3: {

                clearScreen();

                displayHeader(
                    "PRICE RANGE FILTER",
                    "Binary Search"
                );

                double minPrice;
                double maxPrice;

                cout << "Enter minimum price: ";

                if (!(cin >> minPrice)) {

                    cout << "\nInvalid input. Please enter a valid number.\n";

                    cin.clear();
                    cin.ignore(
                        numeric_limits<streamsize>::max(),
                        '\n'
                    );

                    break;
                }

                cout << "Enter maximum price: ";

                if (!(cin >> maxPrice)) {

                    cout << "\nInvalid input. Please enter a valid number.\n";

                    cin.clear();
                    cin.ignore(
                        numeric_limits<streamsize>::max(),
                        '\n'
                    );

                    break;
                }

                cin.ignore(
                    numeric_limits<streamsize>::max(),
                    '\n'
                );

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

                cout << "\nPRICE RANGE RESULTS\n";

                cout << "Price range: "
                     << fixed << setprecision(2)
                     << minPrice << " - " << maxPrice << '\n';

                displayProducts(results);

                break;
            }

            // Lab 8: Inventory sorting.
            case 4: {

                clearScreen();

                displayHeader(
                    "INVENTORY SORTING",
                    "Choose your preferred sorting method"
                );

                printMenuOption(1, "Sort by Brand (Insertion Sort)");
                printMenuOption(2, "Sort by Price (Quick Sort)");
                printMenuOption(3, "Sort by Stock (Quick Sort)");

                cout << '\n';
                printMenuOption(0, "Back to Main Menu", 12);
                cout << '\n';

                int sortChoice;

                if (!readInteger("Enter your choice: ", sortChoice)) {
                    break;
                }

                vector<Item> sortedProducts;

                switch (sortChoice) {

                    case 1:

                        sortedProducts = insertionSortByBrand(products);

                        cout << "\nSORTED BY BRAND\n";
                        displayProducts(sortedProducts);

                        break;

                    case 2:

                        sortedProducts = quickSortByPrice(products);

                        cout << "\nSORTED BY PRICE\n";
                        displayProducts(sortedProducts);

                        break;

                    case 3:

                        sortedProducts = quickSortByStock(products);

                        cout << "\nSORTED BY STOCK\n";
                        displayProducts(sortedProducts);

                        break;

                    case 0:
                        break;

                    default:
                        cout << "\nInvalid sorting option.\n";
                }

                break;
            }

            // Lab 2: Shopping cart.
            case 5: {

                shoppingCartMenu(
                    cart,
                    products,
                    orderQueue,
                    currentUser.username
                );

                break;
            }

            // Lab 3: Brand showcase.
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

                // Synchronize dynamic catalog after checkout.
                try {
                    dynamicCatalog.load(products);
                }
                catch (const bad_alloc&) {

                    cout << "\nWarning: Unable to synchronize "
                         << "the dynamic product catalog.\n";
                }

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

                // Synchronize dynamic catalog after stock changes.
                try {
                    dynamicCatalog.load(products);
                }
                catch (const bad_alloc&) {

                    cout << "\nWarning: Unable to synchronize "
                         << "the dynamic product catalog.\n";
                }

                break;
            }

            // Lab 6: BST product ID search.
            case 9: {

                clearScreen();

                productBSTSearchMenu(
                    productIndex,
                    products
                );

                break;
            }

            // Official Brand Filtering.
            case 10: {

                clearScreen();

                displayHeader(
                    "OFFICIAL BRAND FILTER",
                    "Find products by their official brand"
                );

                try {

                    // Display available brands.
                    displayAvailableBrands(products);

                    cout << '\n';

                    string selectedBrand;

                    cout << "Enter official brand name: ";
                    getline(cin, selectedBrand);

                    // Reject empty or whitespace-only input.
                    if (selectedBrand.find_first_not_of(" \t\n\r") ==
                        string::npos) {

                        cout << "\nBrand name cannot be empty.\n";
                        break;
                    }

                    // Remove unnecessary spaces at the beginning and end.
                    size_t first =
                        selectedBrand.find_first_not_of(" \t\n\r");

                    size_t last =
                        selectedBrand.find_last_not_of(" \t\n\r");

                    selectedBrand =
                        selectedBrand.substr(first, last - first + 1);

                    // Filter products using exact brand matching.
                    vector<Item> filteredProducts =
                        filterProductsByBrand(products, selectedBrand);

                    cout << "\nBRAND FILTER RESULTS\n";

                    cout << "Selected Brand: "
                         << selectedBrand << '\n';

                    if (filteredProducts.empty()) {

                        cout << "\nNo products found for this brand.\n";
                    }
                    else {

                        displayProducts(filteredProducts);
                    }

                }
                catch (const bad_alloc&) {

                    cout << "\nNot enough memory to perform brand filtering.\n";
                }

                break;
            }

            case 0: {

                clearScreen();

                displayHeader(
                    "THANK YOU",
                    "Thank you for using BEAUTRIX!"
                );

                printCentered("Have a beautiful day!", 13);

                break;
            }

            default: {

                cout << "\nInvalid choice. Please try again.\n";

                break;
            }
        }

        if (choice != 0) {
            pauseForUser();
        }

    } while (choice != 0);

    setColor(7);

    return 0;
}
