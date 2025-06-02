#include "../../include/models/Product.h"

Product::Product(int id, std::string name, Category category, double price, int stock)
    : id_(id), 
      name_(std::move(name)),
      category_(category),
      price_(price),
      stock_(stock) {
}

bool Product::sell(int quantity) {
    if(quantity <= 0 || quantity > stock_) return false;
    stock_ -= quantity;
    return true;
}

bool Product::restock(int quantity) {
    if(quantity <= 0) return false;
    stock_ += quantity;
    return true;
}

int Product::id() const noexcept { 
    return id_; 
}

const std::string& Product::name() const noexcept { 
    return name_; 
}

Product::Category Product::category() const noexcept { 
    return category_; 
}

double Product::price() const noexcept { 
    return price_; 
}

int Product::stock() const noexcept { 
    return stock_; 
}

void Product::set_price(double new_price) {
    if(new_price >= 0) {
        price_ = new_price;
    }
}

void Product::set_category(Category new_category) {
    category_ = new_category;
}