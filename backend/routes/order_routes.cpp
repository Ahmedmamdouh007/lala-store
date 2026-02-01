#include <crow.h>
#include "../db/connection.h"
#include "../models/Order.h"
#include "../utils/cors_helper.h"
#include <libpq-fe.h>
#include <nlohmann/json.hpp>
#include <sstream>

using json = nlohmann::json;

void setupOrderRoutes(crow::SimpleApp& app) {
    // Create order from cart
    CROW_ROUTE(app, "/api/orders/create")
    .methods("POST"_method)
    ([](const crow::request& req) {
        try {
            auto body = json::parse(req.body);
            
            int user_id = body["user_id"].get<int>();
            std::string shipping_address = body["shipping_address"].get<std::string>();
            
            auto& db = DatabaseConnection::getInstance();
            PGconn* conn = db.getConnection();
            
            if (!db.isConnected()) {
                return crow::response(500, "Database connection failed");
            }
            
            // Start transaction
            PGresult* beginRes = PQexec(conn, "BEGIN");
            if (PQresultStatus(beginRes) != PGRES_COMMAND_OK) {
                PQclear(beginRes);
                json response;
                response["success"] = false;
                response["message"] = "Failed to start transaction";
                return crow::response(500, response.dump());
            }
            PQclear(beginRes);
            
            // Get cart items and calculate total
            std::string cartQuery = R"(
                SELECT ci.id, ci.product_id, ci.quantity, ci.price
                FROM cart_items ci
                WHERE ci.user_id = $1
            )";
            
            std::string userIdStr = std::to_string(user_id);
            const char* cartParams[1] = {userIdStr.c_str()};
            int cartLengths[1] = {static_cast<int>(userIdStr.length())};
            int cartFormats[1] = {0};
            
            PGresult* cartRes = PQexecParams(conn, cartQuery.c_str(), 1, nullptr, 
                                             cartParams, cartLengths, cartFormats, 0);
            
            if (PQresultStatus(cartRes) != PGRES_TUPLES_OK) {
                PQexec(conn, "ROLLBACK");
                PQclear(cartRes);
                json response;
                response["success"] = false;
                response["message"] = "Failed to get cart items";
                return crow::response(500, response.dump());
            }
            
            int cartRows = PQntuples(cartRes);
            if (cartRows == 0) {
                PQexec(conn, "ROLLBACK");
                PQclear(cartRes);
                json response;
                response["success"] = false;
                response["message"] = "Cart is empty";
                return crow::response(400, response.dump());
            }
            
            double total_amount = 0.0;
            std::vector<int> cartItemIds;
            std::vector<int> productIds;
            std::vector<int> quantities;
            std::vector<double> prices;
            
            for (int i = 0; i < cartRows; i++) {
                cartItemIds.push_back(std::stoi(PQgetvalue(cartRes, i, 0)));
                productIds.push_back(std::stoi(PQgetvalue(cartRes, i, 1)));
                quantities.push_back(std::stoi(PQgetvalue(cartRes, i, 2)));
                double price = std::stod(PQgetvalue(cartRes, i, 3));
                prices.push_back(price);
                total_amount += price * quantities[i];
            }
            PQclear(cartRes);
            
            // Create order
            std::string orderQuery = R"(
                INSERT INTO orders (user_id, total_amount, status, shipping_address)
                VALUES ($1, $2, 'pending', $3)
                RETURNING id
            )";
            
            std::string totalStr = std::to_string(total_amount);
            const char* orderParams[3] = {userIdStr.c_str(), totalStr.c_str(), shipping_address.c_str()};
            int orderLengths[3] = {static_cast<int>(userIdStr.length()), 
                                  static_cast<int>(totalStr.length()),
                                  static_cast<int>(shipping_address.length())};
            int orderFormats[3] = {0, 0, 0};
            
            PGresult* orderRes = PQexecParams(conn, orderQuery.c_str(), 3, nullptr, 
                                             orderParams, orderLengths, orderFormats, 0);
            
            if (PQresultStatus(orderRes) != PGRES_TUPLES_OK) {
                PQexec(conn, "ROLLBACK");
                PQclear(orderRes);
                json response;
                response["success"] = false;
                response["message"] = "Failed to create order";
                return crow::response(500, response.dump());
            }
            
            int order_id = std::stoi(PQgetvalue(orderRes, 0, 0));
            PQclear(orderRes);
            
            // Create order items
            for (size_t i = 0; i < productIds.size(); i++) {
                std::string itemQuery = R"(
                    INSERT INTO order_items (order_id, product_id, quantity, price)
                    VALUES ($1, $2, $3, $4)
                )";
                
                std::string orderIdStr = std::to_string(order_id);
                std::string prodIdStr = std::to_string(productIds[i]);
                std::string qtyStr = std::to_string(quantities[i]);
                std::string priceStr = std::to_string(prices[i]);
                
                const char* itemParams[4] = {orderIdStr.c_str(), prodIdStr.c_str(), 
                                            qtyStr.c_str(), priceStr.c_str()};
                int itemLengths[4] = {static_cast<int>(orderIdStr.length()),
                                     static_cast<int>(prodIdStr.length()),
                                     static_cast<int>(qtyStr.length()),
                                     static_cast<int>(priceStr.length())};
                int itemFormats[4] = {0, 0, 0, 0};
                
                PGresult* itemRes = PQexecParams(conn, itemQuery.c_str(), 4, nullptr, 
                                                itemParams, itemLengths, itemFormats, 0);
                
                if (PQresultStatus(itemRes) != PGRES_COMMAND_OK) {
                    PQexec(conn, "ROLLBACK");
                    PQclear(itemRes);
                    json response;
                    response["success"] = false;
                    response["message"] = "Failed to create order items";
                    return crow::response(500, response.dump());
                }
                PQclear(itemRes);
            }
            
            // Clear cart
            std::string clearCartQuery = "DELETE FROM cart_items WHERE user_id = $1";
            const char* clearParams[1] = {userIdStr.c_str()};
            int clearLengths[1] = {static_cast<int>(userIdStr.length())};
            int clearFormats[1] = {0};
            
            PGresult* clearRes = PQexecParams(conn, clearCartQuery.c_str(), 1, nullptr, 
                                             clearParams, clearLengths, clearFormats, 0);
            
            if (PQresultStatus(clearRes) != PGRES_COMMAND_OK) {
                PQexec(conn, "ROLLBACK");
                PQclear(clearRes);
                json response;
                response["success"] = false;
                response["message"] = "Failed to clear cart";
                return crow::response(500, response.dump());
            }
            PQclear(clearRes);
            
            // Commit transaction
            PGresult* commitRes = PQexec(conn, "COMMIT");
            if (PQresultStatus(commitRes) != PGRES_COMMAND_OK) {
                PQclear(commitRes);
                json response;
                response["success"] = false;
                response["message"] = "Failed to commit transaction";
                return crow::response(500, response.dump());
            }
            PQclear(commitRes);
            
            json response;
            response["success"] = true;
            response["message"] = "Order created successfully";
            response["order_id"] = order_id;
            return crow::response(200, response.dump());
            
        } catch (const std::exception& e) {
            json response;
            response["success"] = false;
            response["message"] = "Invalid request: " + std::string(e.what());
            return crow::response(400, response.dump());
        }
    });
    
    // Get user orders
    CROW_ROUTE(app, "/api/orders/user/<int>")
    .methods("GET"_method)
    ([](int user_id) {
        auto& db = DatabaseConnection::getInstance();
        PGconn* conn = db.getConnection();
        
        if (!db.isConnected()) {
            return crow::response(500, "Database connection failed");
        }
        
        std::string query = R"(
            SELECT id, user_id, total_amount, status, shipping_address, created_at
            FROM orders
            WHERE user_id = $1
            ORDER BY created_at DESC
        )";
        
        std::string userIdStr = std::to_string(user_id);
        const char* paramValues[1] = {userIdStr.c_str()};
        int paramLengths[1] = {static_cast<int>(userIdStr.length())};
        int paramFormats[1] = {0};
        
        PGresult* res = PQexecParams(conn, query.c_str(), 1, nullptr, paramValues, 
                                     paramLengths, paramFormats, 0);
        
        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            PQclear(res);
            return crow::response(500, "Query failed: " + std::string(PQerrorMessage(conn)));
        }
        
        json orders = json::array();
        int rows = PQntuples(res);
        
        for (int i = 0; i < rows; i++) {
            json order;
            order["id"] = std::stoi(PQgetvalue(res, i, 0));
            order["user_id"] = std::stoi(PQgetvalue(res, i, 1));
            order["total_amount"] = std::stod(PQgetvalue(res, i, 2));
            order["status"] = PQgetvalue(res, i, 3);
            order["shipping_address"] = PQgetvalue(res, i, 4);
            order["created_at"] = PQgetvalue(res, i, 5);
            orders.push_back(order);
        }
        
        PQclear(res);
        
        json response;
        response["success"] = true;
        response["data"] = orders;
        
        return CORSHelper::jsonResponse(200, response.dump());
    });
}
