#include <crow.h>
#include "../db/connection.h"
#include "../models/Order.h"
#include "../utils/cors_helper.h"
#include "../utils/stripe_client.h"
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

void setupOrderRoutes(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/api/orders/create")
    .methods("POST"_method)
    ([](const crow::request& req) {
        try {
            auto body = json::parse(req.body);
            int user_id = body["user_id"].get<int>();
            std::string shipping_address = body.value("shipping_address", "");
            std::string customer_name = body.value("customer_name", "");
            std::string phone = body.value("phone", "");
            std::string payment_method = body.value("payment_method", "");
            std::string payment_intent_id = body.value("payment_intent_id", "");

            bool isCardPayment = (payment_method == "card" || payment_method == "visa");
            if (isCardPayment && payment_intent_id.empty()) {
                json e; e["success"]=false; e["message"]="Card payment requires payment verification. Please complete card entry.";
                return CORSHelper::jsonResponse(400, e.dump());
            }
            std::string card_brand, card_last4, currency = "usd";
            if (isCardPayment && !payment_intent_id.empty()) {
                json verify = StripeClient::verifyPaymentIntent(payment_intent_id);
                if (!verify.value("ok", false)) {
                    json e; e["success"]=false; e["message"]="Payment verification failed. Please try again.";
                    return CORSHelper::jsonResponse(400, e.dump());
                }
                json details = StripeClient::getPaymentIntentDetails(payment_intent_id);
                if (details.value("ok", false)) {
                    if (details.contains("card_brand")) card_brand = details["card_brand"].get<std::string>();
                    if (details.contains("card_last4")) card_last4 = details["card_last4"].get<std::string>();
                }
            }

            auto& db = DatabaseConnection::getInstance();
            sqlite3* conn = db.getConnection();
            if (!db.isConnected()) {
                json e; e["success"]=false; e["message"]="Database connection failed";
                return CORSHelper::jsonResponse(500, e.dump());
            }

            if (sqlite3_exec(conn, "BEGIN TRANSACTION", nullptr, nullptr, nullptr) != SQLITE_OK) {
                json e; e["success"]=false; e["message"]="Failed to start transaction";
                return CORSHelper::jsonResponse(500, e.dump());
            }

            sqlite3_stmt* cartStmt = nullptr;
            if (sqlite3_prepare_v2(conn, "SELECT ci.id, ci.product_id, ci.quantity, ci.price FROM cart_items ci WHERE ci.user_id = ?1", -1, &cartStmt, nullptr) != SQLITE_OK) {
                sqlite3_exec(conn, "ROLLBACK", nullptr, nullptr, nullptr);
                json e; e["success"]=false; e["message"]="Failed to get cart items";
                return CORSHelper::jsonResponse(500, e.dump());
            }
            sqlite3_bind_int(cartStmt, 1, user_id);
            std::vector<int> cartItemIds, productIds, quantities;
            std::vector<double> prices;
            double total_amount = 0.0;
            while (sqlite3_step(cartStmt) == SQLITE_ROW) {
                cartItemIds.push_back(col_int(cartStmt, 0));
                productIds.push_back(col_int(cartStmt, 1));
                quantities.push_back(col_int(cartStmt, 2));
                double price = col_double(cartStmt, 3);
                prices.push_back(price);
                total_amount += price * quantities.back();
            }
            sqlite3_finalize(cartStmt);

            if (productIds.empty()) {
                sqlite3_exec(conn, "ROLLBACK", nullptr, nullptr, nullptr);
                json e; e["success"]=false; e["message"]="Cart is empty";
                return CORSHelper::jsonResponse(400, e.dump());
            }

            sqlite3_stmt* orderStmt = nullptr;
            const char* orderSql;
            if (isCardPayment && !payment_intent_id.empty()) {
                orderSql = "INSERT INTO orders (user_id, total_amount, status, shipping_address, customer_name, phone, payment_method, stripe_payment_intent_id, currency, card_brand, card_last4) VALUES (?1, ?2, 'paid', ?3, ?4, ?5, ?6, ?7, ?8, ?9, ?10)";
            } else {
                orderSql = "INSERT INTO orders (user_id, total_amount, status, shipping_address, customer_name, phone, payment_method) VALUES (?1, ?2, 'pending', ?3, ?4, ?5, ?6)";
            }
            if (sqlite3_prepare_v2(conn, orderSql, -1, &orderStmt, nullptr) != SQLITE_OK) {
                sqlite3_exec(conn, "ROLLBACK", nullptr, nullptr, nullptr);
                json e; e["success"]=false; e["message"]="Failed to create order";
                return CORSHelper::jsonResponse(500, e.dump());
            }
            sqlite3_bind_int(orderStmt, 1, user_id);
            sqlite3_bind_double(orderStmt, 2, total_amount);
            sqlite3_bind_text(orderStmt, 3, shipping_address.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(orderStmt, 4, customer_name.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(orderStmt, 5, phone.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(orderStmt, 6, payment_method.c_str(), -1, SQLITE_TRANSIENT);
            if (isCardPayment && !payment_intent_id.empty()) {
                sqlite3_bind_text(orderStmt, 7, payment_intent_id.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(orderStmt, 8, currency.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(orderStmt, 9, card_brand.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_text(orderStmt, 10, card_last4.c_str(), -1, SQLITE_TRANSIENT);
            }
            if (sqlite3_step(orderStmt) != SQLITE_DONE) {
                sqlite3_finalize(orderStmt);
                sqlite3_exec(conn, "ROLLBACK", nullptr, nullptr, nullptr);
                json e; e["success"]=false; e["message"]="Failed to create order";
                return CORSHelper::jsonResponse(500, e.dump());
            }
            sqlite3_finalize(orderStmt);
            sqlite3_int64 order_id = sqlite3_last_insert_rowid(conn);

            sqlite3_stmt* itemStmt = nullptr;
            if (sqlite3_prepare_v2(conn, "INSERT INTO order_items (order_id, product_id, quantity, price) VALUES (?1, ?2, ?3, ?4)", -1, &itemStmt, nullptr) != SQLITE_OK) {
                sqlite3_exec(conn, "ROLLBACK", nullptr, nullptr, nullptr);
                json response;
                response["success"] = false;
                response["message"] = "Failed to create order items";
                return crow::response(500, response.dump());
            }
            for (size_t i = 0; i < productIds.size(); i++) {
                sqlite3_bind_int64(itemStmt, 1, order_id);
                sqlite3_bind_int(itemStmt, 2, productIds[i]);
                sqlite3_bind_int(itemStmt, 3, quantities[i]);
                sqlite3_bind_double(itemStmt, 4, prices[i]);
                if (sqlite3_step(itemStmt) != SQLITE_DONE) {
                    sqlite3_finalize(itemStmt);
                    sqlite3_exec(conn, "ROLLBACK", nullptr, nullptr, nullptr);
                    json response;
                    response["success"] = false;
                    response["message"] = "Failed to create order items";
                    return crow::response(500, response.dump());
                }
                sqlite3_reset(itemStmt);
            }
            sqlite3_finalize(itemStmt);

            sqlite3_stmt* clearStmt = nullptr;
            if (sqlite3_prepare_v2(conn, "DELETE FROM cart_items WHERE user_id = ?1", -1, &clearStmt, nullptr) != SQLITE_OK) {
                sqlite3_exec(conn, "ROLLBACK", nullptr, nullptr, nullptr);
                json response;
                response["success"] = false;
                response["message"] = "Failed to clear cart";
                return crow::response(500, response.dump());
            }
            sqlite3_bind_int(clearStmt, 1, user_id);
            sqlite3_step(clearStmt);
            sqlite3_finalize(clearStmt);

            if (sqlite3_exec(conn, "COMMIT", nullptr, nullptr, nullptr) != SQLITE_OK) {
                json response;
                response["success"] = false;
                response["message"] = "Failed to commit transaction";
                return crow::response(500, response.dump());
            }

            json response;
            response["success"] = true;
            response["message"] = "Order created successfully";
            response["order_id"] = static_cast<int>(order_id);
            return CORSHelper::jsonResponse(200, response.dump());
        } catch (const std::exception& e) {
            json resp; resp["success"]=false; resp["message"]="Invalid request: "+std::string(e.what());
            return CORSHelper::jsonResponse(400, resp.dump());
        }
    });

    CROW_ROUTE(app, "/api/orders/user/<int>")
    .methods("GET"_method)
    ([](int user_id) {
        auto& db = DatabaseConnection::getInstance();
        sqlite3* conn = db.getConnection();
        if (!db.isConnected()) {
            return crow::response(500, "Database connection failed");
        }
        const char* sql = "SELECT id, user_id, total_amount, status, shipping_address, created_at FROM orders WHERE user_id = ?1 ORDER BY created_at DESC";
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr) != SQLITE_OK) {
            return crow::response(500, "Query failed");
        }
        sqlite3_bind_int(stmt, 1, user_id);
        json orders = json::array();
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            json order;
            order["id"] = col_int(stmt, 0);
            order["user_id"] = col_int(stmt, 1);
            order["total_amount"] = col_double(stmt, 2);
            order["status"] = col_text(stmt, 3);
            order["shipping_address"] = col_text(stmt, 4);
            order["created_at"] = col_text(stmt, 5);
            orders.push_back(order);
        }
        sqlite3_finalize(stmt);
        json response;
        response["success"] = true;
        response["data"] = orders;
        return CORSHelper::jsonResponse(200, response.dump());
    });

    CROW_ROUTE(app, "/api/orders/by-status")
    .methods("GET"_method)
    ([](const crow::request& req) {
        std::string status = req.url_params.get("status") ? req.url_params.get("status") : "pending";
        auto& db = DatabaseConnection::getInstance();
        sqlite3* conn = db.getConnection();
        if (!db.isConnected()) {
            return crow::response(500, "Database connection failed");
        }
        const char* sql = "SELECT id, user_id, total_amount, status, shipping_address, created_at FROM orders WHERE status = ?1 ORDER BY created_at DESC";
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr) != SQLITE_OK) {
            return crow::response(500, "Query failed");
        }
        sqlite3_bind_text(stmt, 1, status.c_str(), -1, SQLITE_TRANSIENT);
        json orders = json::array();
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            json order;
            order["id"] = col_int(stmt, 0);
            order["user_id"] = col_int(stmt, 1);
            order["total_amount"] = col_double(stmt, 2);
            order["status"] = col_text(stmt, 3);
            order["shipping_address"] = col_text(stmt, 4);
            order["created_at"] = col_text(stmt, 5);
            orders.push_back(order);
        }
        sqlite3_finalize(stmt);
        json response;
        response["success"] = true;
        response["data"] = orders;
        return CORSHelper::jsonResponse(200, response.dump());
    });
}
