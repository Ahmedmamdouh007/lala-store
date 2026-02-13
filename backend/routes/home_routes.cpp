#include <crow.h>
#include "../db/connection.h"
#include "../models/Product.h"
#include "../utils/cors_helper.h"
#include <sqlite3.h>
#include <nlohmann/json.hpp>
#include <vector>

using json = nlohmann::json;

namespace {
    const char* col_text(sqlite3_stmt* stmt, int col) {
        const char* p = reinterpret_cast<const char*>(sqlite3_column_text(stmt, col));
        return p ? p : "";
    }
    int col_int(sqlite3_stmt* stmt, int col) { return sqlite3_column_int(stmt, col); }
    double col_double(sqlite3_stmt* stmt, int col) { return sqlite3_column_double(stmt, col); }
}

void setupHomeRoutes(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/api/home/featured")
    ([]() {
        try {
            auto& db = DatabaseConnection::getInstance();
            sqlite3* conn = db.getConnection();
            if (!db.isConnected()) {
                json e; e["success"]=false; e["message"]="Database connection failed";
                return CORSHelper::jsonResponse(500, e.dump());
            }
            const char* sql = "SELECT p.id, p.name, p.description, p.price, p.image_url, "
                       "p.category_id, c.name, p.gender, COALESCE(p.stock_quantity,0), p.created_at, p.sizes, p.size_chart "
                       "FROM products p LEFT JOIN categories c ON p.category_id = c.id "
                       "WHERE COALESCE(p.stock_quantity,0) > 0 ORDER BY p.created_at DESC LIMIT 8";
            sqlite3_stmt* stmt = nullptr;
            if (sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr) != SQLITE_OK) {
                json e; e["success"]=false; e["message"]="Query failed";
                return CORSHelper::jsonResponse(500, e.dump());
            }
            json products = json::array();
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                json product;
                product["id"] = col_int(stmt, 0);
                product["name"] = col_text(stmt, 1);
                product["description"] = col_text(stmt, 2);
                product["price"] = col_double(stmt, 3);
                product["image_url"] = col_text(stmt, 4);
                product["category_id"] = col_int(stmt, 5);
                product["category_name"] = col_text(stmt, 6);
                product["gender"] = col_text(stmt, 7);
                product["stock_quantity"] = col_int(stmt, 8);
                product["created_at"] = col_text(stmt, 9);
                product["sizes"] = col_text(stmt, 10);
                product["size_chart"] = col_text(stmt, 11);
                products.push_back(product);
            }
            sqlite3_finalize(stmt);
            json response;
            response["success"] = true;
            response["data"] = products;
            return CORSHelper::jsonResponse(200, response.dump());
        } catch (const std::exception& ex) {
            json e; e["success"]=false; e["message"]=std::string("Error: ")+ex.what();
            return CORSHelper::jsonResponse(500, e.dump());
        }
    });

    CROW_ROUTE(app, "/api/home/categories")
    ([]() {
        auto& db = DatabaseConnection::getInstance();
        sqlite3* conn = db.getConnection();
        if (!db.isConnected()) {
            return crow::response(500, "Database connection failed");
        }
        const char* sql = "SELECT id, name, description FROM categories ORDER BY name";
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr) != SQLITE_OK) {
            return crow::response(500, "Query failed");
        }
        json categories = json::array();
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            json category;
            category["id"] = col_int(stmt, 0);
            category["name"] = col_text(stmt, 1);
            category["description"] = col_text(stmt, 2);
            categories.push_back(category);
        }
        sqlite3_finalize(stmt);
        json response;
        response["success"] = true;
        response["data"] = categories;
        return CORSHelper::jsonResponse(200, response.dump());
    });

    CROW_ROUTE(app, "/api/home/search")
    ([](const crow::request& req) {
        std::string q = req.url_params.get("q") ? req.url_params.get("q") : "";
        try {
            auto& db = DatabaseConnection::getInstance();
            sqlite3* conn = db.getConnection();
            if (!db.isConnected()) {
                json e; e["success"]=false; e["message"]="Database connection failed";
                return CORSHelper::jsonResponse(500, e.dump());
            }
            std::string like = "%" + q + "%";
            const char* sql = "SELECT p.id, p.name, p.description, p.price, p.image_url, "
                "p.category_id, c.name, p.gender, COALESCE(p.stock_quantity,0), p.created_at, p.sizes, p.size_chart "
                "FROM products p LEFT JOIN categories c ON p.category_id = c.id "
                "WHERE p.name LIKE ?1 AND COALESCE(p.stock_quantity,0) > 0 ORDER BY p.name";
            sqlite3_stmt* stmt = nullptr;
            if (sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr) != SQLITE_OK) {
                json e; e["success"]=false; e["message"]="Query failed";
                return CORSHelper::jsonResponse(500, e.dump());
            }
            sqlite3_bind_text(stmt, 1, like.c_str(), -1, SQLITE_TRANSIENT);
            json products = json::array();
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                json product;
                product["id"] = col_int(stmt, 0);
                product["name"] = col_text(stmt, 1);
                product["description"] = col_text(stmt, 2);
                product["price"] = col_double(stmt, 3);
                product["image_url"] = col_text(stmt, 4);
                product["category_id"] = col_int(stmt, 5);
                product["category_name"] = col_text(stmt, 6);
                product["gender"] = col_text(stmt, 7);
                product["stock_quantity"] = col_int(stmt, 8);
                product["created_at"] = col_text(stmt, 9);
                product["sizes"] = col_text(stmt, 10);
                product["size_chart"] = col_text(stmt, 11);
                products.push_back(product);
            }
            sqlite3_finalize(stmt);
            json response;
            response["success"] = true;
            response["data"] = products;
            return CORSHelper::jsonResponse(200, response.dump());
        } catch (const std::exception& ex) {
            json e; e["success"]=false; e["message"]=std::string("Error: ")+ex.what();
            return CORSHelper::jsonResponse(500, e.dump());
        }
    });
}
