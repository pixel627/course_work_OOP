#pragma once

#include <string>

class Product {
public:
    enum class Category { Food, Drink, Accessory, Service };
    
    Product(int id, std::string name, Category category, double price, int stock);
    
    bool sell(int quantity);
    bool restock(int quantity);
    
    int id() const noexcept;
    const std::string& name() const noexcept;
    Category category() const noexcept;
    double price() const noexcept;
    int stock() const noexcept;

    void set_price(double new_price);
    void set_category(Category new_category);

private:
    int id_;
    std::string name_;
    Category category_;
    double price_;
    int stock_;
};