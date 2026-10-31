
#pragma once

#include <string>

class Item {
private:
    int id;
    std::string name;
    std::string brand;
    std::string category;
    double price;
    int stock;

public:
    Item(int id, const std::string& name,
         const std::string& brand,
         const std::string& category,
         double price, int stock);

    int getId() const;
    std::string getName() const;
    std::string getBrand() const;
    std::string getCategory() const;
    double getPrice() const;
    int getStock() const;
};
