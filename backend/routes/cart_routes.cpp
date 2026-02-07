#include <crow.h>
#include "../db/connection.h"
#include "../models/Cart.h"
#include "../utils/cors_helper.h"
#include <libpq-fe.h>
#include <nlohmann/json.hpp>
#include <sstream>

using json = nlohmann::json;

namespace {
    int safe_stoi(const char* v, int def = 0) {
        if (!v || !*v) return def;
        try { return std::stoi(v); } catch (...) { return def; }
    }
    double safe_stod(const char* v, double def = 0) {
        if (!v || !*v) return def;
        try { return std::stod(v); } catch (...) { return def; }
    }
    const char* safe_str(const char* v) { return (v && *v) ? v : ""; }
}

void setupCartRoutes(crow::SimpleApp& app) {
    // Get cart items for a user (using session/user_id)
    CROW_ROUTE(app, "/api/cart/<int>")
    .methods("GET"_method)
    ([](int user_id) {
        auto& db = DatabaseConnection::getInstance();
        PGconn* conn = db.getConnection();
        
        if (!db.isConnected()) {
            json r; r["success"]=false; r["message"]="Database connection failed";
            return CORSHelper::jsonResponse(500, r.dump());
        }
        
        std::string query = R"(
            SELECT ci.id, ci.user_id, ci.product_id, ci.quantity, ci.price, ci.created_at,
                   p.name as product_name, p.image_url as product_image
            FROM cart_items ci
            JOIN products p ON ci.product_id = p.id
            WHERE ci.user_id = $1
            ORDER BY ci.created_at DESC
        )";
        
        std::string idStr = std::to_string(user_id);
        const char* paramValues[1] = {idStr.c_str()};
        int paramLengths[1] = {static_cast<int>(idStr.length())};
        int paramFormats[1] = {0};
        
        PGresult* res = PQexecParams(conn, query.c_str(), 1, nullptr, paramValues, 
                                     paramLengths, paramFormats, 0);
        
        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            const char* err = PQresultErrorMessage(res);
            PQclear(res);
            json r; r["success"]=false; r["message"]=std::string("Query failed: ")+(err?err:PQerrorMessage(conn));
            return CORSHelper::jsonResponse(500, r.dump());
        }
        
        json cartItems = json::array();
        int rows = PQntuples(res);
        
        for (int i = 0; i < rows; i++) {
            json item;
            item["id"] = safe_stoi(PQgetvalue(res, i, 0));
            item["user_id"] = safe_stoi(PQgetvalue(res, i, 1));
            item["product_id"] = safe_stoi(PQgetvalue(res, i, 2));
            item["quantity"] = safe_stoi(PQgetvalue(res, i, 3));
            item["price"] = safe_stod(PQgetvalue(res, i, 4));
            item["created_at"] = safe_str(PQgetvalue(res, i, 5));
            item["product_name"] = safe_str(PQgetvalue(res, i, 6));
            item["product_image"] = safe_str(PQgetvalue(res, i, 7));
            cartItems.push_back(item);
        }
        
        PQclear(res);
        
        json response;
        response["success"] = true;
        response["data"] = cartItems;
        
        return CORSHelper::jsonResponse(200, response.dump());
    });
    
    // Add item to cart
    CROW_ROUTE(app, "/api/cart/add")
    .methods("POST"_method)
    ([](const crow::request& req) {
        try {
            auto body = json::parse(req.body);
            
            int user_id = body["user_id"].get<int>();
            int product_id = body["product_id"].get<int>();
            int quantity = 1;
            if (body.contains("quantity") && !body["quantity"].is_null()) {
                try { quantity = body["quantity"].get<int>(); } catch (...) {}
                if (quantity < 1) quantity = 1;
            }
            
            auto& db = DatabaseConnection::getInstance();
            PGconn* conn = db.getConnection();
            
            if (!db.isConnected()) {
                json r; r["success"]=false; r["message"]="Database connection failed";
                return CORSHelper::jsonResponse(500, r.dump());
            }
            
            // Get product price
            std::string priceQuery = "SELECT price FROM products WHERE id = $1";
            std::string prodIdStr = std::to_string(product_id);
            const char* priceParams[1] = {prodIdStr.c_str()};
            int priceLengths[1] = {static_cast<int>(prodIdStr.length())};
            int priceFormats[1] = {0};
            
            PGresult* priceRes = PQexecParams(conn, priceQuery.c_str(), 1, nullptr, 
                                             priceParams, priceLengths, priceFormats, 0);
            
            if (PQresultStatus(priceRes) != PGRES_TUPLES_OK || PQntuples(priceRes) == 0) {
                PQclear(priceRes);
                json response;
                response["success"] = false;
                response["message"] = "Product not found";
                return CORSHelper::jsonResponse(404, response.dump());
            }
            
            double price = safe_stod(PQgetvalue(priceRes, 0, 0));
            PQclear(priceRes);
            
            // Check if item already exists in cart
            std::string checkQuery = "SELECT id, quantity FROM cart_items WHERE user_id = $1 AND product_id = $2";
            std::string userIdStr = std::to_string(user_id);
            const char* checkParams[2] = {userIdStr.c_str(), prodIdStr.c_str()};
            int checkLengths[2] = {static_cast<int>(userIdStr.length()), static_cast<int>(prodIdStr.length())};
            int checkFormats[2] = {0, 0};
            
            PGresult* checkRes = PQexecParams(conn, checkQuery.c_str(), 2, nullptr, 
                                              checkParams, checkLengths, checkFormats, 0);
            
            if (PQresultStatus(checkRes) == PGRES_TUPLES_OK && PQntuples(checkRes) > 0) {
                // Update existing item
                int existingId = safe_stoi(PQgetvalue(checkRes, 0, 0));
                int existingQty = safe_stoi(PQgetvalue(checkRes, 0, 1));
                PQclear(checkRes);
                
                int newQty = existingQty + quantity;
                std::string updateQuery = "UPDATE cart_items SET quantity = $1 WHERE id = $2";
                std::string newQtyStr = std::to_string(newQty);
                std::string existingIdStr = std::to_string(existingId);
                const char* updateParams[2] = {newQtyStr.c_str(), existingIdStr.c_str()};
                int updateLengths[2] = {static_cast<int>(newQtyStr.length()), static_cast<int>(existingIdStr.length())};
                int updateFormats[2] = {0, 0};
                
                PGresult* updateRes = PQexecParams(conn, updateQuery.c_str(), 2, nullptr, 
                                                   updateParams, updateLengths, updateFormats, 0);
                
                if (PQresultStatus(updateRes) != PGRES_COMMAND_OK) {
                    const char* err = PQresultErrorMessage(updateRes);
                    PQclear(updateRes);
                    json response;
                    response["success"] = false;
                    response["message"] = std::string("Failed to update cart: ") + (err ? err : "");
                    return CORSHelper::jsonResponse(500, response.dump());
                }
                PQclear(updateRes);
            } else {
                PQclear(checkRes);
                
                // Insert new item
                std::string insertQuery = R"(
                    INSERT INTO cart_items (user_id, product_id, quantity, price)
                    VALUES ($1, $2, $3, $4)
                    RETURNING id
                )";
                
                std::string qtyStr = std::to_string(quantity);
                std::string priceStr = std::to_string(price);
                const char* insertParams[4] = {userIdStr.c_str(), prodIdStr.c_str(), 
                                               qtyStr.c_str(), priceStr.c_str()};
                int insertLengths[4] = {static_cast<int>(userIdStr.length()), 
                                       static_cast<int>(prodIdStr.length()),
                                       static_cast<int>(qtyStr.length()),
                                       static_cast<int>(priceStr.length())};
                int insertFormats[4] = {0, 0, 0, 0};
                
                PGresult* insertRes = PQexecParams(conn, insertQuery.c_str(), 4, nullptr, 
                                                   insertParams, insertLengths, insertFormats, 0);
                
                if (PQresultStatus(insertRes) != PGRES_TUPLES_OK) {
                    const char* err = PQresultErrorMessage(insertRes);
                    PQclear(insertRes);
                    json response;
                    response["success"] = false;
                    response["message"] = std::string("Failed to add to cart: ") + (err ? err : "");
                    return CORSHelper::jsonResponse(500, response.dump());
                }
                PQclear(insertRes);
            }
            
            json response;
            response["success"] = true;
            response["message"] = "Item added to cart";
            return CORSHelper::jsonResponse(200, response.dump());
            
        } catch (const std::exception& e) {
            json response;
            response["success"] = false;
            response["message"] = "Invalid request: " + std::string(e.what());
            return CORSHelper::jsonResponse(400, response.dump());
        }
    });
    
    // Remove item from cart
    CROW_ROUTE(app, "/api/cart/remove/<int>")
    .methods("DELETE"_method)
    ([](int cart_item_id) {
        auto& db = DatabaseConnection::getInstance();
        PGconn* conn = db.getConnection();
        
        if (!db.isConnected()) {
            json r; r["success"]=false; r["message"]="Database connection failed";
            return CORSHelper::jsonResponse(500, r.dump());
        }
        
        std::string query = "DELETE FROM cart_items WHERE id = $1";
        std::string idStr = std::to_string(cart_item_id);
        const char* paramValues[1] = {idStr.c_str()};
        int paramLengths[1] = {static_cast<int>(idStr.length())};
        int paramFormats[1] = {0};
        
        PGresult* res = PQexecParams(conn, query.c_str(), 1, nullptr, paramValues, 
                                     paramLengths, paramFormats, 0);
        
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            PQclear(res);
            json response;
            response["success"] = false;
            response["message"] = "Failed to remove item";
            return CORSHelper::jsonResponse(500, response.dump());
        }
        
        PQclear(res);
        
        json response;
        response["success"] = true;
        response["message"] = "Item removed from cart";
        return CORSHelper::jsonResponse(200, response.dump());
    });
    
    // Update cart item quantity
    CROW_ROUTE(app, "/api/cart/update")
    .methods("PUT"_method)
    ([](const crow::request& req) {
        try {
            auto body = json::parse(req.body);
            
            int cart_item_id = body["cart_item_id"].get<int>();
            int quantity = body["quantity"].get<int>();
            
            if (quantity <= 0) {
                json response;
                response["success"] = false;
                response["message"] = "Quantity must be greater than 0";
                return CORSHelper::jsonResponse(400, response.dump());
            }
            
            auto& db = DatabaseConnection::getInstance();
            PGconn* conn = db.getConnection();
            
            if (!db.isConnected()) {
                json r; r["success"]=false; r["message"]="Database connection failed";
                return CORSHelper::jsonResponse(500, r.dump());
            }
            
            std::string query = "UPDATE cart_items SET quantity = $1 WHERE id = $2";
            std::string qtyStr = std::to_string(quantity);
            std::string idStr = std::to_string(cart_item_id);
            const char* paramValues[2] = {qtyStr.c_str(), idStr.c_str()};
            int paramLengths[2] = {static_cast<int>(qtyStr.length()), static_cast<int>(idStr.length())};
            int paramFormats[2] = {0, 0};
            
            PGresult* res = PQexecParams(conn, query.c_str(), 2, nullptr, paramValues, 
                                         paramLengths, paramFormats, 0);
            
            if (PQresultStatus(res) != PGRES_COMMAND_OK) {
                PQclear(res);
                json response;
                response["success"] = false;
                response["message"] = "Failed to update cart";
                return CORSHelper::jsonResponse(500, response.dump());
            }
            
            PQclear(res);
            
            json response;
            response["success"] = true;
            response["message"] = "Cart updated";
            return CORSHelper::jsonResponse(200, response.dump());
            
        } catch (const std::exception& e) {
            json response;
            response["success"] = false;
            response["message"] = "Invalid request: " + std::string(e.what());
            return CORSHelper::jsonResponse(400, response.dump());
        }
    });
}
