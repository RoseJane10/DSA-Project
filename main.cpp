
#include "Database.h"
#include "InventoryRepository.h"
#include "DSAComponents.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <limits>

using namespace std;

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
             << setw(30) << product.getName()
             << setw(25) << product.getBrand()
             << setw(15) << product.getCategory()
             << setw(12) << fixed << setprecision(2)
             << product.getPrice()
             << product.getStock()
             << "\n";
    }

    cout << "\nTotal Products: " << products.size() << "\n";
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

    int choice;

    do {

        cout << "\n============================================\n";
        cout << "             BEAUTRIX SYSTEM\n";
        cout << "============================================\n";

        cout << "1. View Complete Catalog\n";
        cout << "2. Search Products by Keyword\n";
        cout << "0. Exit\n";

        cout << "============================================\n";
        cout << "Enter your choice: ";

        if (!(cin >> choice)) {
            cout << "\nInvalid input. Please enter a number.\n";

            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {

            case 1: {

                cout << "\n========== COMPLETE PRODUCT CATALOG ==========\n";

                displayProducts(products);

                break;
            }

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

            case 0:

                cout << "\nThank you for using BEAUTRIX!\n";
                break;

            default:

                cout << "\nInvalid choice. Please try again.\n";
        }

    } while (choice != 0);

    return 0;
}
