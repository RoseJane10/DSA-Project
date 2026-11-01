
#include "OrderQueue.h"

#include <iostream>
using namespace std;

int main() {
    OrderQueue queue;

    // Create sample products.
    Item product1(1, "Fit Me Foundation", "Maybelline",
                  "Makeup", 499.00, 10);

    Item product2(2, "Lip Tint", "GRWM Cosmetics",
                  "Makeup", 299.00, 15);

    Item product3(3, "Niacinamide Serum", "The Ordinary",
                  "Skincare", 650.00, 8);

    // Create three customer orders.
    CustomerOrder order1;
    order1.username = "customer1";
    order1.items.push_back({product1, 2});

    CustomerOrder order2;
    order2.username = "customer2";
    order2.items.push_back({product2, 1});

    CustomerOrder order3;
    order3.username = "customer3";
    order3.items.push_back({product3, 3});

    cout << "===== ORDER QUEUE TEST =====\n";

    // Test empty queue.
    cout << "\nIs queue empty? "
         << (queue.isEmpty() ? "Yes" : "No") << "\n";

    // Add orders.
    cout << "\nAdding orders...\n";

    queue.enqueue(order1);
    queue.enqueue(order2);
    queue.enqueue(order3);

    // Test queue size.
    cout << "\nQueue size: " << queue.getSize() << "\n";

    // Display all orders.
    queue.displayQueue();

    // Test peek.
    CustomerOrder nextOrder;

    if (queue.peek(nextOrder)) {
        cout << "\nNext order belongs to: "
             << nextOrder.username << "\n";
    }

    // Test FIFO processing.
    cout << "\n===== PROCESSING ORDERS =====\n";

    CustomerOrder processedOrder;

    while (queue.dequeue(processedOrder)) {
        cout << "Processing order of: "
             << processedOrder.username << "\n";
    }

    // Test empty queue again.
    cout << "\nFinal queue size: " << queue.getSize() << "\n";

    cout << "Is queue empty? "
         << (queue.isEmpty() ? "Yes" : "No") << "\n";

    return 0;
}
