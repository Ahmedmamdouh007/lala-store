#include <crow.h>
#include "../db/connection.h"
#include "../models/Cart.h"
#include "../utils/cors_helper.h"
#include <sqlite3.h>
#include <nlohmann/json.hpp>
#include <sstream>

using json = nlohmann::json;

namespace {
    const char* col_text(sqlite3_stmt* stmt, int col) {
        const char* p = reinterpret_cast<const char*>(sqlite3_column_text(stmt, col));
        return p ? p : "";
    }
    int col_int(sqlite3_stmt* stmt, int col) { return sqlite3_column_int(stmt, col); }
    double col_double(sqlite3_stmt* stmt, int col) { return sqlite3_column_double(stmt, col); }
}

void setupCartRoutes(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/api/cart/<int>")
    .methods("GET"_method)
    ([](int user_id) {
        auto& db = DatabaseConnection::getInstance();
        sqlite3* conn = db.getConnection();
        if (!db.isConnected()) {
            json r; r["success"]=false; r["message"]="Database connection failed";
            return CORSHelper::jsonResponse(500, r.dump());
        }
        const char* sql = "SELECT ci.id, ci.user_id, ci.product_id, ci.quantity, ci.price, ci.created_at, "
                   "p.name, p.image_url FROM cart_items ci JOIN products p ON ci.product_id = p.id "
                   "WHERE ci.user_id = ?1 ORDER BY ci.created_at DESC";
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr) != SQLITE_OK) {
            json r; r["success"]=false; r["message"]="Query failed";
            return CORSHelper::jsonResponse(500, r.dump());
        }
        sqlite3_bind_int(stmt, 1, user_id);
        json cartItems = json::array();
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            json item;
            item["id"] = col_int(stmt, 0);
            item["user_id"] = col_int(stmt, 1);
            item["product_id"] = col_int(stmt, 2);
            item["quantity"] = col_int(stmt, 3);
            item["price"] = col_double(stmt, 4);
            item["created_at"] = col_text(stmt, 5);
            item["product_name"] = col_text(stmt, 6);
            item["product_image"] = col_text(stmt, 7);
            cartItems.push_back(item);
        }
        sqlite3_finalize(stmt);
        json response;
        response["success"] = true;
        response["data"] = cartItems;
        return CORSHelper::jsonResponse(200, response.dump());
    });

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
            sqlite3* conn = db.getConnection();
            if (!db.isConnected()) {
                json r; r["success"]=false; r["message"]="Database connection failed";
                return CORSHelper::jsonResponse(500, r.dump());
            }

            sqlite3_stmt* stmt = nullptr;
            if (sqlite3_prepare_v2(conn, "SELECT id FROM users WHERE id = ?1", -1, &stmt, nullptr) != SQLITE_OK) {
                json response;
                response["success"] = false;
                response["message"] = "Database error";
                return CORSHelper::jsonResponse(500, response.dump());
            }
            sqlite3_bind_int(stmt, 1, user_id);
            bool userExists = (sqlite3_step(stmt) == SQLITE_ROW);
            sqlite3_finalize(stmt);
            if (!userExists) {
                json response;
                response["success"] = false;
                response["message"] = "User not found. Please ensure user_id exists in the database.";
                return CORSHelper::jsonResponse(404, response.dump());
            }

            if (sqlite3_prepare_v2(conn, "SELECT price FROM products WHERE id = ?1", -1, &stmt, nullptr) != SQLITE_OK) {
                json response;
                response["success"] = false;
                response["message"] = "Database error";
                return CORSHelper::jsonResponse(500, response.dump());
            }
            sqlite3_bind_int(stmt, 1, product_id);
            if (sqlite3_step(stmt) != SQLITE_ROW) {
                sqlite3_finalize(stmt);
                json response;
                response["success"] = false;
                response["message"] = "Product not found";
                return CORSHelper::jsonResponse(404, response.dump());
            }
            double price = col_double(stmt, 0);
            sqlite3_finalize(stmt);

            if (sqlite3_prepare_v2(conn, "SELECT id, quantity FROM cart_items WHERE user_id = ?1 AND product_id = ?2", -1, &stmt, nullptr) != SQLITE_OK) {
                json response;
                response["success"] = false;
                response["message"] = "Database error";
                return CORSHelper::jsonResponse(500, response.dump());
            }
            sqlite3_bind_int(stmt, 1, user_id);
            sqlite3_bind_int(stmt, 2, product_id);
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                int existingId = col_int(stmt, 0);
                int existingQty = col_int(stmt, 1);
                sqlite3_finalize(stmt);
                int newQty = existingQty + quantity;
                sqlite3_stmt* up = nullptr;
                if (sqlite3_prepare_v2(conn, "UPDATE cart_items SET quantity = ?1 WHERE id = ?2", -1, &up, nullptr) != SQLITE_OK) {
                    json response;
                    response["success"] = false;
                    response["message"] = "Failed to update cart";
                    return CORSHelper::jsonResponse(500, response.dump());
                }
                sqlite3_bind_int(up, 1, newQty);
                sqlite3_bind_int(up, 2, existingId);
                int rc = sqlite3_step(up);
                sqlite3_finalize(up);
                if (rc != SQLITE_DONE) {
                    json response;
                    response["success"] = false;
                    response["message"] = "Failed to update cart";
                    return CORSHelper::jsonResponse(500, response.dump());
                }
            } else {
                sqlite3_finalize(stmt);
                sqlite3_stmt* ins = nullptr;
                if (sqlite3_prepare_v2(conn, "INSERT INTO cart_items (user_id, product_id, quantity, price) VALUES (?1, ?2, ?3, ?4)", -1, &ins, nullptr) != SQLITE_OK) {
                    json response;
                    response["success"] = false;
                    response["message"] = "Failed to add to cart";
                    return CORSHelper::jsonResponse(500, response.dump());
                }
                sqlite3_bind_int(ins, 1, user_id);
                sqlite3_bind_int(ins, 2, product_id);
                sqlite3_bind_int(ins, 3, quantity);
                sqlite3_bind_double(ins, 4, price);
                int rc = sqlite3_step(ins);
                sqlite3_finalize(ins);
                if (rc != SQLITE_DONE) {
                    const char* err = sqlite3_errmsg(conn);
                    std::string errorMsg = err ? err : "Unknown error";
                    if (errorMsg.find("foreign key") != std::string::npos) {
                        json response;
                        response["success"] = false;
                        response["message"] = "Invalid user or product. Please ensure both exist in the database.";
                        return CORSHelper::jsonResponse(400, response.dump());
                    }
                    json response;
                    response["success"] = false;
                    response["message"] = "Failed to add to cart: " + errorMsg;
                    return CORSHelper::jsonResponse(500, response.dump());
                }
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

    CROW_ROUTE(app, "/api/cart/remove/<int>")
    .methods("DELETE"_method)
    ([](int cart_item_id) {
        auto& db = DatabaseConnection::getInstance();
        sqlite3* conn = db.getConnection();
        if (!db.isConnected()) {
            json r; r["success"]=false; r["message"]="Database connection failed";
            return CORSHelper::jsonResponse(500, r.dump());
        }
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(conn, "DELETE FROM cart_items WHERE id = ?1", -1, &stmt, nullptr) != SQLITE_OK) {
            json response;
            response["success"] = false;
            response["message"] = "Failed to remove item";
            return CORSHelper::jsonResponse(500, response.dump());
        }
        sqlite3_bind_int(stmt, 1, cart_item_id);
        int rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        if (rc != SQLITE_DONE) {
            json response;
            response["success"] = false;
            response["message"] = "Failed to remove item";
            return CORSHelper::jsonResponse(500, response.dump());
        }
        json response;
        response["success"] = true;
        response["message"] = "Item removed from cart";
        return CORSHelper::jsonResponse(200, response.dump());
    });

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
            sqlite3* conn = db.getConnection();
            if (!db.isConnected()) {
                json r; r["success"]=false; r["message"]="Database connection failed";
                return CORSHelper::jsonResponse(500, r.dump());
            }
            sqlite3_stmt* stmt = nullptr;
            if (sqlite3_prepare_v2(conn, "UPDATE cart_items SET quantity = ?1 WHERE id = ?2", -1, &stmt, nullptr) != SQLITE_OK) {
                json response;
                response["success"] = false;
                response["message"] = "Failed to update cart";
                return CORSHelper::jsonResponse(500, response.dump());
            }
            sqlite3_bind_int(stmt, 1, quantity);
            sqlite3_bind_int(stmt, 2, cart_item_id);
            int rc = sqlite3_step(stmt);
            sqlite3_finalize(stmt);
            if (rc != SQLITE_DONE) {
                json response;
                response["success"] = false;
                response["message"] = "Failed to update cart";
                return CORSHelper::jsonResponse(500, response.dump());
            }
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
