#include "Database.h"
#include <iostream>

int main() {
    std::cout << "Starting BEAUTRIX..." << std::endl;

    Database database;

    if (!database.connect()) {
        std::cerr << "PostgreSQL connection failed!" << std::endl;
        return 1;
    }

    std::cout << "PostgreSQL connected successfully!" << std::endl;

    return 0;
}