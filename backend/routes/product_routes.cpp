#include <crow.h>
#include "../db/connection.h"
#include "../models/Product.h"
#include "../utils/cors_helper.h"
#include <libpq-fe.h>
#include <nlohmann/json.hpp>
#include <sstream>

using json = nlohmann::json;

void setupProductRoutes(crow::SimpleApp& app) {
    // Get all products
    CROW_ROUTE(app, "/api/products")
    ([]() {
        auto& db = DatabaseConnection::getInstance();
        PGconn* conn = db.getConnection();
        
        if (!db.isConnected()) {
            return crow::response(500, "Database connection failed");
        }
        
        const char* query = R"(
            SELECT p.id, p.name, p.description, p.price, p.image_url, 
                   p.category_id, c.name as category_name, p.gender, 
                   p.stock_quantity, p.created_at
            FROM products p
            LEFT JOIN categories c ON p.category_id = c.id
            WHERE p.stock_quantity > 0
            ORDER BY p.created_at DESC
        )";
        
        PGresult* res = PQexec(conn, query);
        
        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            PQclear(res);
            return crow::response(500, "Query failed: " + std::string(PQerrorMessage(conn)));
        }
        
        json products = json::array();
        int rows = PQntuples(res);
        
        for (int i = 0; i < rows; i++) {
            json product;
            product["id"] = std::stoi(PQgetvalue(res, i, 0));
            product["name"] = PQgetvalue(res, i, 1);
            product["description"] = PQgetvalue(res, i, 2);
            product["price"] = std::stod(PQgetvalue(res, i, 3));
            product["image_url"] = PQgetvalue(res, i, 4);
            product["category_id"] = std::stoi(PQgetvalue(res, i, 5));
            product["category_name"] = PQgetvalue(res, i, 6) ? PQgetvalue(res, i, 6) : "";
            product["gender"] = PQgetvalue(res, i, 7);
            product["stock_quantity"] = std::stoi(PQgetvalue(res, i, 8));
            product["created_at"] = PQgetvalue(res, i, 9);
            products.push_back(product);
        }
        
        PQclear(res);
        
        json response;
        response["success"] = true;
        response["data"] = products;
        
        return CORSHelper::jsonResponse(200, response.dump());
    });
    
    // Get products by gender
    CROW_ROUTE(app, "/api/products/<string>")
    ([](const std::string& gender) {
        auto& db = DatabaseConnection::getInstance();
        PGconn* conn = db.getConnection();
        
        if (!db.isConnected()) {
            return crow::response(500, "Database connection failed");
        }
        
        std::string query = R"(
            SELECT p.id, p.name, p.description, p.price, p.image_url, 
                   p.category_id, c.name as category_name, p.gender, 
                   p.stock_quantity, p.created_at
            FROM products p
            LEFT JOIN categories c ON p.category_id = c.id
            WHERE p.gender = $1 AND p.stock_quantity > 0
            ORDER BY p.created_at DESC
        )";
        
        const char* paramValues[1] = {gender.c_str()};
        int paramLengths[1] = {static_cast<int>(gender.length())};
        int paramFormats[1] = {0};
        
        PGresult* res = PQexecParams(conn, query.c_str(), 1, nullptr, paramValues, 
                                     paramLengths, paramFormats, 0);
        
        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            PQclear(res);
            return crow::response(500, "Query failed: " + std::string(PQerrorMessage(conn)));
        }
        
        json products = json::array();
        int rows = PQntuples(res);
        
        for (int i = 0; i < rows; i++) {
            json product;
            product["id"] = std::stoi(PQgetvalue(res, i, 0));
            product["name"] = PQgetvalue(res, i, 1);
            product["description"] = PQgetvalue(res, i, 2);
            product["price"] = std::stod(PQgetvalue(res, i, 3));
            product["image_url"] = PQgetvalue(res, i, 4);
            product["category_id"] = std::stoi(PQgetvalue(res, i, 5));
            product["category_name"] = PQgetvalue(res, i, 6) ? PQgetvalue(res, i, 6) : "";
            product["gender"] = PQgetvalue(res, i, 7);
            product["stock_quantity"] = std::stoi(PQgetvalue(res, i, 8));
            product["created_at"] = PQgetvalue(res, i, 9);
            products.push_back(product);
        }
        
        PQclear(res);
        
        json response;
        response["success"] = true;
        response["data"] = products;
        
        return crow::response(200, response.dump());
    });
    
    // Get single product by ID
    CROW_ROUTE(app, "/api/products/details/<int>")
    ([](int product_id) {
        auto& db = DatabaseConnection::getInstance();
        PGconn* conn = db.getConnection();
        
        if (!db.isConnected()) {
            return crow::response(500, "Database connection failed");
        }
        
        std::string query = R"(
            SELECT p.id, p.name, p.description, p.price, p.image_url, 
                   p.category_id, c.name as category_name, p.gender, 
                   p.stock_quantity, p.created_at
            FROM products p
            LEFT JOIN categories c ON p.category_id = c.id
            WHERE p.id = $1
        )";
        
        std::string idStr = std::to_string(product_id);
        const char* paramValues[1] = {idStr.c_str()};
        int paramLengths[1] = {static_cast<int>(idStr.length())};
        int paramFormats[1] = {0};
        
        PGresult* res = PQexecParams(conn, query.c_str(), 1, nullptr, paramValues, 
                                     paramLengths, paramFormats, 0);
        
        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            PQclear(res);
            return crow::response(500, "Query failed: " + std::string(PQerrorMessage(conn)));
        }
        
        if (PQntuples(res) == 0) {
            PQclear(res);
            json response;
            response["success"] = false;
            response["message"] = "Product not found";
            return crow::response(404, response.dump());
        }
        
        json product;
        product["id"] = std::stoi(PQgetvalue(res, 0, 0));
        product["name"] = PQgetvalue(res, 0, 1);
        product["description"] = PQgetvalue(res, 0, 2);
        product["price"] = std::stod(PQgetvalue(res, 0, 3));
        product["image_url"] = PQgetvalue(res, 0, 4);
        product["category_id"] = std::stoi(PQgetvalue(res, 0, 5));
        product["category_name"] = PQgetvalue(res, 0, 6) ? PQgetvalue(res, 0, 6) : "";
        product["gender"] = PQgetvalue(res, 0, 7);
        product["stock_quantity"] = std::stoi(PQgetvalue(res, 0, 8));
        product["created_at"] = PQgetvalue(res, 0, 9);
        
        PQclear(res);
        
        json response;
        response["success"] = true;
        response["data"] = product;
        
        return CORSHelper::jsonResponse(200, response.dump());
    });
}
