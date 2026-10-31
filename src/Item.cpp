
#include "Item.h"

Item::Item(int id, const std::string& name,
           const std::string& brand,
           const std::string& category,
           double price, int stock) {

    this->id = id;
    this->name = name;
    this->brand = brand;
    this->category = category;
    this->price = price;
    this->stock = stock;
}

int Item::getId() const {
    return id;
}

std::string Item::getName() const {
    return name;
}

std::string Item::getBrand() const {
    return brand;
}

std::string Item::getCategory() const {
    return category;
}

double Item::getPrice() const {
    return price;
}

int Item::getStock() const {
    return stock;
}
