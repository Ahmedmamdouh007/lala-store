#ifndef CART_H
#define CART_H

#include <string>

struct CartItem {
    int id;
    int user_id;
    int product_id;
    int quantity;
    double price;
    std::string created_at;
    
    // Joined product data
    std::string product_name;
    std::string product_image;
};

#endif // CART_H
