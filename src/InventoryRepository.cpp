
#include "InventoryRepository.h"

#include <iostream>
#include <string>
#include <exception>

// Automatically releases PostgreSQL query results.
namespace {
    class ResultGuard {
    private:
        Database& database;
        PGresult* result;

    public:
        ResultGuard(Database& db, PGresult* res)
            : database(db), result(res) {}

        ~ResultGuard() {
            database.finish(result);
        }

        PGresult* get() const {
            return result;
        }

        ResultGuard(const ResultGuard&) = delete;
        ResultGuard& operator=(const ResultGuard&) = delete;
    };
}

InventoryRepository::InventoryRepository(Database& database)
    : database(database) {
}

// Retrieve all products from PostgreSQL.
std::vector<Item> InventoryRepository::getAllProducts() {

    std::vector<Item> products;

    std::string sql =
        "SELECT p.id, p.name, b.name AS brand, "
        "p.category, p.price, p.stock "
        "FROM products p "
        "JOIN brands b ON p.brand_id = b.id "
        "ORDER BY p.id;";

    ResultGuard result(database, database.exec(sql));

    if (!result.get() ||
        PQresultStatus(result.get()) != PGRES_TUPLES_OK) {

        std::cerr << "Failed to retrieve products!\n";

        if (result.get()) {
            std::cerr << PQresultErrorMessage(result.get());
        }

        return products;
    }

    for (int i = 0; i < PQntuples(result.get()); i++) {

        int id = std::stoi(PQgetvalue(result.get(), i, 0));
        std::string name = PQgetvalue(result.get(), i, 1);
        std::string brand = PQgetvalue(result.get(), i, 2);
        std::string category = PQgetvalue(result.get(), i, 3);
        double price = std::stod(PQgetvalue(result.get(), i, 4));
        int stock = std::stoi(PQgetvalue(result.get(), i, 5));

        products.emplace_back(
            id, name, brand, category, price, stock
        );
    }

    return products;
}

// Update product stock in PostgreSQL.
bool InventoryRepository::updateProductStock(
    int productId,
    int expectedStock,
    int newStock
) {

    if (productId <= 0 || expectedStock < 0 || newStock < 0) {
        std::cout << "\nInvalid product ID or stock quantity.\n";
        return false;
    }

    std::string sql =
        "UPDATE products "
        "SET stock = $3 "
        "WHERE id = $1 AND stock = $2 "
        "RETURNING id;";

    std::string idValue = std::to_string(productId);
    std::string expectedValue = std::to_string(expectedStock);
    std::string newValue = std::to_string(newStock);

    const char* values[] = {
        idValue.c_str(),
        expectedValue.c_str(),
        newValue.c_str()
    };

    ResultGuard result(
        database,
        database.params(sql, 3, values)
    );

    if (!result.get() ||
        PQresultStatus(result.get()) != PGRES_TUPLES_OK) {

        std::cerr << "\nFailed to update product stock!\n";

        if (result.get()) {
            std::cerr << PQresultErrorMessage(result.get());
        }

        return false;
    }

    bool updated = PQntuples(result.get()) == 1;

    if (!updated) {
        std::cout
            << "\nStock update failed. The product may not exist "
            << "or its stock may have changed.\n";
        return false;
    }

    return true;
}

// Process an order using a PostgreSQL transaction.
bool InventoryRepository::processOrder(
    const CustomerOrder& order,
    int& orderId,
    double& orderTotal
) {

    orderId = 0;
    orderTotal = 0.0;

    // Validate the order before starting a transaction.
    if (order.username.empty() ||
        order.username.size() > 40 ||
        order.items.empty()) {

        std::cout << "\nInvalid order information.\n";
        return false;
    }

    for (const OrderLineItem& item : order.items) {
        if (item.item.getId() <= 0 || item.quantity <= 0) {
            std::cout << "\nInvalid product ID or quantity.\n";
            return false;
        }
    }

    // Start transaction.
    {
        ResultGuard beginResult(database, database.exec("BEGIN;"));

        if (!beginResult.get() ||
            PQresultStatus(beginResult.get()) != PGRES_COMMAND_OK) {

            std::cerr << "\nCould not start checkout transaction.\n";
            return false;
        }
    }

    bool transactionActive = true;

    // Roll back all changes if any operation fails.
    auto rollback = [&]() {
        if (transactionActive) {
            ResultGuard rollbackResult(
                database,
                database.exec("ROLLBACK;")
            );

            transactionActive = false;

            std::cout << "\nCheckout transaction rolled back.\n";
        }
    };

    try {

        // Create the order record.
        std::string insertOrderSQL =
            "INSERT INTO orders (username, total) "
            "VALUES ($1, 0) "
            "RETURNING id;";

        const char* orderValues[] = {
            order.username.c_str()
        };

        int newOrderId = 0;

        {
            ResultGuard result(
                database,
                database.params(insertOrderSQL, 1, orderValues)
            );

            if (!result.get() ||
                PQresultStatus(result.get()) != PGRES_TUPLES_OK ||
                PQntuples(result.get()) != 1) {

                std::cerr << "\nFailed to create order.\n";

                if (result.get()) {
                    std::cerr << PQresultErrorMessage(result.get());
                }

                rollback();
                return false;
            }

            newOrderId = std::stoi(
                PQgetvalue(result.get(), 0, 0)
            );
        }

        // Process each product in the order.
        for (const OrderLineItem& line : order.items) {

            int productId = line.item.getId();
            int quantity = line.quantity;

            std::string productIdValue =
                std::to_string(productId);

            // Lock the product row and retrieve its current price
            // and stock directly from PostgreSQL.
            std::string selectSQL =
                "SELECT price::text, stock "
                "FROM products "
                "WHERE id = $1 "
                "FOR UPDATE;";

            const char* selectValues[] = {
                productIdValue.c_str()
            };

            std::string currentPrice;
            int currentStock = 0;

            {
                ResultGuard result(
                    database,
                    database.params(selectSQL, 1, selectValues)
                );

                if (!result.get() ||
                    PQresultStatus(result.get()) != PGRES_TUPLES_OK ||
                    PQntuples(result.get()) != 1) {

                    std::cout
                        << "\nProduct ID " << productId
                        << " does not exist. Checkout cancelled.\n";

                    rollback();
                    return false;
                }

                currentPrice = PQgetvalue(result.get(), 0, 0);

                currentStock = std::stoi(
                    PQgetvalue(result.get(), 0, 1)
                );
            }

            // Verify current stock.
            if (currentStock < quantity) {

                std::cout
                    << "\nInsufficient stock for product ID "
                    << productId << ".\n";

                rollback();
                return false;
            }

            // Deduct product stock.
            std::string quantityValue =
                std::to_string(quantity);

            std::string updateStockSQL =
                "UPDATE products "
                "SET stock = stock - $2 "
                "WHERE id = $1 AND stock >= $2 "
                "RETURNING id;";

            const char* stockValues[] = {
                productIdValue.c_str(),
                quantityValue.c_str()
            };

            {
                ResultGuard result(
                    database,
                    database.params(
                        updateStockSQL,
                        2,
                        stockValues
                    )
                );

                if (!result.get() ||
                    PQresultStatus(result.get()) != PGRES_TUPLES_OK ||
                    PQntuples(result.get()) != 1) {

                    std::cerr
                        << "\nFailed to deduct product stock.\n";

                    rollback();
                    return false;
                }
            }

            // Save the order item using the current database price.
            std::string orderIdValue =
                std::to_string(newOrderId);

            std::string insertItemSQL =
                "INSERT INTO order_items "
                "(order_id, product_id, quantity, unit_price) "
                "VALUES ($1, $2, $3, $4);";

            const char* itemValues[] = {
                orderIdValue.c_str(),
                productIdValue.c_str(),
                quantityValue.c_str(),
                currentPrice.c_str()
            };

            {
                ResultGuard result(
                    database,
                    database.params(
                        insertItemSQL,
                        4,
                        itemValues
                    )
                );

                if (!result.get() ||
                    PQresultStatus(result.get()) != PGRES_COMMAND_OK) {

                    std::cerr
                        << "\nFailed to save order item.\n";

                    if (result.get()) {
                        std::cerr
                            << PQresultErrorMessage(result.get());
                    }

                    rollback();
                    return false;
                }
            }
        }

        // Calculate the final total using PostgreSQL NUMERIC values.
        std::string totalSQL =
            "UPDATE orders "
            "SET total = ("
            "    SELECT COALESCE(SUM(quantity * unit_price), 0) "
            "    FROM order_items "
            "    WHERE order_id = $1"
            ") "
            "WHERE id = $1 "
            "RETURNING total::text;";

        std::string orderIdValue =
            std::to_string(newOrderId);

        const char* totalValues[] = {
            orderIdValue.c_str()
        };

        {
            ResultGuard result(
                database,
                database.params(totalSQL, 1, totalValues)
            );

            if (!result.get() ||
                PQresultStatus(result.get()) != PGRES_TUPLES_OK ||
                PQntuples(result.get()) != 1) {

                std::cerr
                    << "\nFailed to calculate order total.\n";

                rollback();
                return false;
            }

            orderTotal = std::stod(
                PQgetvalue(result.get(), 0, 0)
            );
        }

        // Commit all changes.
        {
            ResultGuard commitResult(
                database,
                database.exec("COMMIT;")
            );

            if (!commitResult.get() ||
                PQresultStatus(commitResult.get()) != PGRES_COMMAND_OK) {

                std::cerr << "\nFailed to commit checkout.\n";

                rollback();
                return false;
            }
        }

        transactionActive = false;
        orderId = newOrderId;

        std::cout << "\nCheckout completed successfully!\n";

        return true;
    }
    catch (const std::exception& error) {

        std::cerr
            << "\nCheckout error: "
            << error.what() << "\n";

        rollback();

        orderId = 0;
        orderTotal = 0.0;

        return false;
    }
}
