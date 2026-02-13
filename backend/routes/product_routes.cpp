#include <crow.h>
#include "../db/connection.h"
#include "../models/Product.h"
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

void setupProductRoutes(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/api/products/details/<int>")
    ([](int product_id) {
        auto& db = DatabaseConnection::getInstance();
        sqlite3* conn = db.getConnection();
        if (!db.isConnected()) {
            json e; e["success"]=false; e["message"]="Database connection failed";
            return CORSHelper::jsonResponse(500, e.dump());
        }
        const char* sql = "SELECT p.id, p.name, p.description, p.price, p.image_url, "
                   "p.category_id, c.name, p.gender, COALESCE(p.stock_quantity,0), p.created_at, p.sizes, p.size_chart "
                   "FROM products p LEFT JOIN categories c ON p.category_id = c.id WHERE p.id = ?1";
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr) != SQLITE_OK) {
            json e; e["success"]=false; e["message"]="Query failed";
            return CORSHelper::jsonResponse(500, e.dump());
        }
        sqlite3_bind_int(stmt, 1, product_id);
        int step = sqlite3_step(stmt);
        if (step != SQLITE_ROW) {
            sqlite3_finalize(stmt);
            json resp;
            resp["success"] = false;
            resp["message"] = "Product not found";
            return crow::response(404, resp.dump());
        }
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
        sqlite3_finalize(stmt);
        json resp;
        resp["success"] = true;
        resp["data"] = product;
        return CORSHelper::jsonResponse(200, resp.dump());
    });

    CROW_ROUTE(app, "/api/products")
    ([]() {
        auto& db = DatabaseConnection::getInstance();
        sqlite3* conn = db.getConnection();
        if (!db.isConnected()) {
            json e; e["success"]=false; e["message"]="Database connection failed";
            return CORSHelper::jsonResponse(500, e.dump());
        }
        const char* sql = "SELECT p.id, p.name, p.description, p.price, p.image_url, "
                   "p.category_id, c.name, p.gender, COALESCE(p.stock_quantity,0), p.created_at, p.sizes, p.size_chart "
                   "FROM products p LEFT JOIN categories c ON p.category_id = c.id "
                   "WHERE COALESCE(p.stock_quantity,0) > 0 ORDER BY p.created_at DESC";
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
    });

    CROW_ROUTE(app, "/api/products/<string>")
    ([](const std::string& gender) {
        auto& db = DatabaseConnection::getInstance();
        sqlite3* conn = db.getConnection();
        if (!db.isConnected()) {
            json e; e["success"]=false; e["message"]="Database connection failed";
            return CORSHelper::jsonResponse(500, e.dump());
        }
        const char* sql = "SELECT p.id, p.name, p.description, p.price, p.image_url, "
                   "p.category_id, c.name, p.gender, COALESCE(p.stock_quantity,0), p.created_at, p.sizes, p.size_chart "
                   "FROM products p LEFT JOIN categories c ON p.category_id = c.id "
                   "WHERE p.gender = ?1 AND COALESCE(p.stock_quantity,0) > 0 ORDER BY p.created_at DESC";
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr) != SQLITE_OK) {
            json e; e["success"]=false; e["message"]="Query failed";
            return CORSHelper::jsonResponse(500, e.dump());
        }
        sqlite3_bind_text(stmt, 1, gender.c_str(), -1, SQLITE_TRANSIENT);
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
    });

    CROW_ROUTE(app, "/api/products/category/<string>")
    ([](const std::string& categoryName) {
        auto& db = DatabaseConnection::getInstance();
        sqlite3* conn = db.getConnection();
        if (!db.isConnected()) {
            json e; e["success"]=false; e["message"]="Database connection failed";
            return CORSHelper::jsonResponse(500, e.dump());
        }
        const char* sql = "SELECT p.id, p.name, p.description, p.price, p.image_url, "
                   "p.category_id, c.name, p.gender, COALESCE(p.stock_quantity,0), p.created_at, p.sizes, p.size_chart "
                   "FROM products p LEFT JOIN categories c ON p.category_id = c.id "
                   "WHERE c.name = ?1 AND COALESCE(p.stock_quantity,0) > 0 ORDER BY p.name";
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(conn, sql, -1, &stmt, nullptr) != SQLITE_OK) {
            json e; e["success"]=false; e["message"]="Query failed";
            return CORSHelper::jsonResponse(500, e.dump());
        }
        sqlite3_bind_text(stmt, 1, categoryName.c_str(), -1, SQLITE_TRANSIENT);
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
    });
}
