# BEAUTRIX
### A Beauty Cosmetics Inventory Management and Intelligent Product Search System

BEAUTRIX is a C++17-based inventory management system designed to manage beauty and cosmetic products using different Data Structures and Algorithms (DSA). It uses PostgreSQL as its database to store product information, user accounts, brands, and order records.

The system provides product searching, filtering, sorting, shopping cart management, checkout processing, and inventory management with role-based access.

---

## 1. Project Objectives

- Manage beauty and cosmetic product inventory.
- Organize products according to their official brands.
- Provide efficient product searching, filtering, and sorting.
- Apply the eight DSA laboratory concepts in a practical system.
- Store inventory and transaction records using PostgreSQL.
- Implement role-based access for administrators, staff, and regular users.

## 2. Technologies Used

- **Programming Language:** C++17
- **Database:** PostgreSQL
- **Database Library:** libpq
- **Build System:** CMake
- **Compiler:** MinGW-w64 GCC
- **Development Environment:** Visual Studio Code
- **Version Control:** Git

## 3. Product Categories

BEAUTRIX includes different beauty and cosmetics brands, organized into categories such as:

- International Makeup Brands
- Skincare Brands
- Filipino Beauty Brands
- Premium and Luxury Beauty Brands

Each product contains information such as its product ID, product name, official brand, category, price, and available stock.

## 4. System Features

| Menu | Feature |
|---|---|
| 1 | View Complete Catalog |
| 2 | Search Products by Keyword |
| 3 | Filter Products by Price Range |
| 4 | Sort Inventory |
| 5 | Shopping Cart |
| 6 | Partner Brand Showcase |
| 7 | Checkout Order Queue |
| 8 | Stock Management and Undo |
| 9 | Search Product by ID |
| 10 | Filter Products by Official Brand |
| 0 | Exit BEAUTRIX |

### Feature Descriptions

**Product Catalog**
- Displays the complete product inventory.
- Shows product details and brand affiliations.
- Uses dynamic memory allocation to manage catalog capacity.

**Product Searching and Filtering**
- Searches products using keywords.
- Filters products according to minimum and maximum prices.
- Searches for products by ID using a Binary Search Tree.
- Filters products by their official brand names.

**Sorting**
- Sorts inventory by price.
- Sorts inventory by available stock.
- Sorts inventory by brand.

**Shopping Cart**
- Allows users to add and manage products in their cart.
- Uses a doubly linked list for cart navigation.
- Supports wishlist functionality.

**Partner Brand Showcase**
- Displays participating beauty brands.
- Uses a circular linked list for continuous brand navigation.

**Checkout and Order Processing**
- Processes checkout requests using a FIFO queue.
- Saves completed order information to PostgreSQL.
- Updates product stock after successful checkout.

**Stock Management**
- Allows authorized administrators and staff to manage inventory.
- Uses a stack to support LIFO-based stock operation undo.
- Restricts inventory management access for regular users.

## 5. Implementation of the Eight DSA Laboratories

| Laboratory | Data Structure / Algorithm | BEAUTRIX Implementation |
|---|---|---|
| Lab 1 | Dynamic Memory Allocation | Dynamic Product Catalog |
| Lab 2 | Singly and Doubly Linked Lists | Wishlist and Shopping Cart |
| Lab 3 | Circular Linked List | Partner Brand Showcase |
| Lab 4 | Stack (LIFO) | Inventory Management Undo |
| Lab 5 | Queue (FIFO) | Checkout Order Processing |
| Lab 6 | Binary Search Tree | Product ID Search |
| Lab 7 | Searching Algorithms | Linear Keyword Search and Binary Price Search |
| Lab 8 | Sorting Algorithms | Sorting by Price, Stock, and Brand |

## 6. User Roles and Access Control

BEAUTRIX supports three user roles:

| Role | Access |
|---|---|
| Admin | Full system access, including stock management |
| Staff | Authorized inventory management functions |
| User | Product browsing, searching, filtering, cart, and checkout |

The stock management option is displayed as restricted for regular users.

## 7. Project Structure

```text
DSA-Project/
│
├── database/
│   └── schema.sql
│
├── include/
│   ├── Database.h
│   ├── Item.h
│   ├── InventoryRepository.h
│   ├── DSAComponents.h
│   ├── ShoppingCart.h
│   ├── BrandShowcase.h
│   ├── InventoryUndoStack.h
│   ├── AuthSystem.h
│   ├── OrderQueue.h
│   ├── ProductBST.h
│   └── DynamicProductCatalog.h
│
├── src/
│   ├── Database.cpp
│   ├── Item.cpp
│   ├── InventoryRepository.cpp
│   ├── DSAComponents.cpp
│   ├── ShoppingCart.cpp
│   ├── BrandShowcase.cpp
│   ├── InventoryUndoStack.cpp
│   ├── AuthSystem.cpp
│   ├── OrderQueue.cpp
│   ├── ProductBST.cpp
│   └── DynamicProductCatalog.cpp
│
├── main.cpp
├── CMakeLists.txt
├── README.md
├── run.ps1
├── .gitignore
└── .env
```

Generated build files are stored in the `build/` directory.

## 8. Database Configuration

BEAUTRIX uses PostgreSQL for persistent data storage.

The database schema is located at:

```text
database/schema.sql
```

The database contains the following main tables:

- `users` – Stores user account information and roles.
- `brands` – Stores official brand information.
- `products` – Stores product details and inventory.
- `orders` – Stores completed order records.
- `order_items` – Stores the products included in each order.

### Environment Variables

The application reads its PostgreSQL connection string from a `.env` file in the project root.

Example format:

```env
BEAUTRIX_DB=host=localhost port=5432 dbname=beautrix user=postgres password=YOUR_PASSWORD
```

Replace `YOUR_PASSWORD` with your own PostgreSQL password.

**Security Note:** Never upload or commit your actual `.env` file to GitHub. The file should remain excluded through `.gitignore`.

## 9. Requirements

Before running BEAUTRIX, make sure you have:

1. Visual Studio Code
2. MinGW-w64 GCC with C++17 support
3. CMake
4. PostgreSQL
5. PostgreSQL development libraries (libpq)
6. Git

## 10. Building the Project

Open PowerShell in the project directory.

Configure CMake:

```powershell
cmake -S . -B build -G "MinGW Makefiles"
```

Build the application:

```powershell
cmake --build build
```

## 11. Running BEAUTRIX

The project includes a PowerShell launcher named `run.ps1`.

Run:

```powershell
.\run.ps1
```

The launcher loads the environment configuration and starts the compiled application.

If PowerShell blocks script execution, use:

```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
```

Then run the launcher again.

## 12. Testing

The system should be tested for:

- Product catalog display.
- Product searching and brand filtering.
- Price-range filtering.
- Inventory sorting.
- Shopping cart and wishlist operations.
- Circular brand showcase navigation.
- Inventory undo operations with authorized roles.
- FIFO checkout processing.
- Product ID searching.
- PostgreSQL connection and order storage.
- Stock updates after successful checkout.
- Role-based access restrictions.

## 13. Version Control

Git is used to track project development and feature implementation.

The project follows a feature-by-feature commit approach to keep changes organized and easier to review.

Example:

```powershell
git status
git add .
git commit -m "feat: describe implemented feature"
```

---

## 14. Project Summary

BEAUTRIX demonstrates how fundamental Data Structures and Algorithms can be applied to a practical beauty cosmetics inventory management system.

**Project:** BEAUTRIX  
**Course:** Data Structures and Algorithms  
**Language:** C++17  
**Database:** PostgreSQL