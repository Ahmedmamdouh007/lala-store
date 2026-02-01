#ifndef ORDER_H
#define ORDER_H

#include <string>

struct Order {
    int id;
    int user_id;
    double total_amount;
    std::string status; // "pending", "processing", "shipped", "delivered"
    std::string shipping_address;
    std::string created_at;
};

struct OrderItem {
    int id;
    int order_id;
    int product_id;
    int quantity;
    double price;
    std::string product_name;
};

#endif // ORDER_H
