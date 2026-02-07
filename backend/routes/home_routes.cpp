#include <crow.h>
#include "../db/connection.h"
#include "../models/Product.h"
#include "../utils/cors_helper.h"
#include <libpq-fe.h>
#include <nlohmann/json.hpp>
#include <vector>

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

void setupHomeRoutes(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/api/home/featured")
    ([]() {
        try {
            auto& db = DatabaseConnection::getInstance();
            PGconn* conn = db.getConnection();
            if (!db.isConnected()) {
                json e; e["success"]=false; e["message"]="Database connection failed";
                return CORSHelper::jsonResponse(500, e.dump());
            }
            const char* query = R"(
                SELECT p.id, p.name, p.description, p.price, p.image_url,
                       p.category_id, c.name as category_name, p.gender,
                       COALESCE(p.stock_quantity, 0), p.created_at
                FROM products p
                LEFT JOIN categories c ON p.category_id = c.id
                WHERE COALESCE(p.stock_quantity, 0) > 0
                ORDER BY p.created_at DESC
                LIMIT 8
            )";
            PGresult* res = PQexec(conn, query);
            if (PQresultStatus(res) != PGRES_TUPLES_OK) {
                json e; e["success"]=false; e["message"]="Query failed: "+std::string(PQerrorMessage(conn));
                PQclear(res);
                return CORSHelper::jsonResponse(500, e.dump());
            }
            json products = json::array();
            int rows = PQntuples(res);
            for (int i = 0; i < rows; i++) {
                json product;
                product["id"] = safe_stoi(PQgetvalue(res, i, 0));
                product["name"] = safe_str(PQgetvalue(res, i, 1));
                product["description"] = safe_str(PQgetvalue(res, i, 2));
                product["price"] = safe_stod(PQgetvalue(res, i, 3));
                product["image_url"] = safe_str(PQgetvalue(res, i, 4));
                product["category_id"] = safe_stoi(PQgetvalue(res, i, 5));
                product["category_name"] = safe_str(PQgetvalue(res, i, 6));
                product["gender"] = safe_str(PQgetvalue(res, i, 7));
                product["stock_quantity"] = safe_stoi(PQgetvalue(res, i, 8));
                product["created_at"] = safe_str(PQgetvalue(res, i, 9));
                products.push_back(product);
            }
            PQclear(res);
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
            category["id"] = safe_stoi(PQgetvalue(res, i, 0));
            category["name"] = safe_str(PQgetvalue(res, i, 1));
            category["description"] = safe_str(PQgetvalue(res, i, 2));
            categories.push_back(category);
        }
        
        PQclear(res);
        
        json response;
        response["success"] = true;
        response["data"] = categories;
        
        return CORSHelper::jsonResponse(200, response.dump());
    });
}
