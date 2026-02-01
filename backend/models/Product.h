#ifndef PRODUCT_H
#define PRODUCT_H

#include <string>

struct Product {
    int id;
    std::string name;
    std::string description;
    double price;
    std::string image_url;
    int category_id;
    std::string category_name;
    std::string gender; // "men" or "women"
    int stock_quantity;
    std::string created_at;
};

#endif // PRODUCT_H
