#include <crow.h>
#include "../db/connection.h"
#include "../models/Product.h"
#include "../utils/cors_helper.h"
#include <libpq-fe.h>
#include <nlohmann/json.hpp>
#include <vector>

using json = nlohmann::json;

void setupHomeRoutes(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/api/home/featured")
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
            LIMIT 8
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
    
    CROW_ROUTE(app, "/api/home/categories")
    ([]() {
        auto& db = DatabaseConnection::getInstance();
        PGconn* conn = db.getConnection();
        
        if (!db.isConnected()) {
            return crow::response(500, "Database connection failed");
        }
        
        const char* query = "SELECT id, name, description FROM categories ORDER BY name";
        
        PGresult* res = PQexec(conn, query);
        
        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            PQclear(res);
            return crow::response(500, "Query failed: " + std::string(PQerrorMessage(conn)));
        }
        
        json categories = json::array();
        int rows = PQntuples(res);
        
        for (int i = 0; i < rows; i++) {
            json category;
            category["id"] = std::stoi(PQgetvalue(res, i, 0));
            category["name"] = PQgetvalue(res, i, 1);
            category["description"] = PQgetvalue(res, i, 2) ? PQgetvalue(res, i, 2) : "";
            categories.push_back(category);
        }
        
        PQclear(res);
        
        json response;
        response["success"] = true;
        response["data"] = categories;
        
        return CORSHelper::jsonResponse(200, response.dump());
    });
}
