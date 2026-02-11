#include <crow.h>
#include "../db/connection.h"
#include "../models/Product.h"
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

void setupProductRoutes(crow::SimpleApp& app) {
    // Get single product by ID - MUST be before /api/products/<string>
    CROW_ROUTE(app, "/api/products/details/<int>")
    ([](int product_id) {
        auto& db = DatabaseConnection::getInstance();
        PGconn* conn = db.getConnection();
        if (!db.isConnected()) {
            json e; e["success"]=false; e["message"]="Database connection failed";
            return CORSHelper::jsonResponse(500, e.dump());
        }
        std::string query = R"(
            SELECT p.id, p.name, p.description, p.price, p.image_url,
                   p.category_id, c.name as category_name, p.gender,
                   p.stock_quantity, p.created_at, p.sizes, p.size_chart
            FROM products p LEFT JOIN categories c ON p.category_id = c.id
            WHERE p.id = $1
        )";
        std::string idStr = std::to_string(product_id);
        const char* paramValues[1] = {idStr.c_str()};
        int paramLengths[1] = {static_cast<int>(idStr.length())};
        int paramFormats[1] = {0};
        PGresult* res = PQexecParams(conn, query.c_str(), 1, nullptr, paramValues, paramLengths, paramFormats, 0);
        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            json e; e["success"]=false; e["message"]="Query failed: "+std::string(PQerrorMessage(conn));
            PQclear(res);
            return CORSHelper::jsonResponse(500, e.dump());
        }
        if (PQntuples(res) == 0) {
            PQclear(res);
            json resp;
            resp["success"] = false;
            resp["message"] = "Product not found";
            return crow::response(404, resp.dump());
        }
        json product;
        product["id"] = safe_stoi(PQgetvalue(res, 0, 0));
        product["name"] = safe_str(PQgetvalue(res, 0, 1));
        product["description"] = safe_str(PQgetvalue(res, 0, 2));
        product["price"] = safe_stod(PQgetvalue(res, 0, 3));
        product["image_url"] = safe_str(PQgetvalue(res, 0, 4));
        product["category_id"] = safe_stoi(PQgetvalue(res, 0, 5));
        product["category_name"] = safe_str(PQgetvalue(res, 0, 6));
        product["gender"] = safe_str(PQgetvalue(res, 0, 7));
        product["stock_quantity"] = safe_stoi(PQgetvalue(res, 0, 8));
        product["created_at"] = safe_str(PQgetvalue(res, 0, 9));
        product["sizes"] = safe_str(PQgetvalue(res, 0, 10));
        product["size_chart"] = safe_str(PQgetvalue(res, 0, 11));
        PQclear(res);
        json resp;
        resp["success"] = true;
        resp["data"] = product;
        return CORSHelper::jsonResponse(200, resp.dump());
    });

    // Get all products
    CROW_ROUTE(app, "/api/products")
    ([]() {
        auto& db = DatabaseConnection::getInstance();
        PGconn* conn = db.getConnection();
        
        if (!db.isConnected()) {
            json e; e["success"]=false; e["message"]="Database connection failed";
            return CORSHelper::jsonResponse(500, e.dump());
        }
        const char* query = R"(
            SELECT p.id, p.name, p.description, p.price, p.image_url, 
                   p.category_id, c.name as category_name, p.gender, 
                   COALESCE(p.stock_quantity, 0), p.created_at, p.sizes, p.size_chart
            FROM products p
            LEFT JOIN categories c ON p.category_id = c.id
            WHERE COALESCE(p.stock_quantity, 0) > 0
            ORDER BY p.created_at DESC
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
            product["sizes"] = safe_str(PQgetvalue(res, i, 10));
            product["size_chart"] = safe_str(PQgetvalue(res, i, 11));
            products.push_back(product);
        }
        PQclear(res);
        json response;
        response["success"] = true;
        response["data"] = products;
        return CORSHelper::jsonResponse(200, response.dump());
    });

    // Get products by gender
    // INTENTIONALLY VULNERABLE FOR ACADEMIC SECURITY TESTING (localhost lab only)
    // SQL injection: query built by string concatenation, no parameter binding
    CROW_ROUTE(app, "/api/products/<string>")
    ([](const std::string& gender) {
        auto& db = DatabaseConnection::getInstance();
        PGconn* conn = db.getConnection();
        
        if (!db.isConnected()) {
            json e; e["success"]=false; e["message"]="Database connection failed";
            return CORSHelper::jsonResponse(500, e.dump());
        }
        std::string query = "SELECT p.id, p.name, p.description, p.price, p.image_url, "
                   "p.category_id, c.name as category_name, p.gender, "
                   "COALESCE(p.stock_quantity, 0), p.created_at, p.sizes, p.size_chart "
                   "FROM products p "
                   "LEFT JOIN categories c ON p.category_id = c.id "
                   "WHERE p.gender = '" + gender + "' AND COALESCE(p.stock_quantity, 0) > 0 "
                   "ORDER BY p.created_at DESC";
        
        PGresult* res = PQexec(conn, query.c_str());
        
        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            const char* err = PQresultErrorMessage(res);
            json e; e["success"]=false; e["message"]="Query failed: "+std::string(err ? err : PQerrorMessage(conn));
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
            product["sizes"] = safe_str(PQgetvalue(res, i, 10));
            product["size_chart"] = safe_str(PQgetvalue(res, i, 11));
            products.push_back(product);
        }
        PQclear(res);
        json response;
        response["success"] = true;
        response["data"] = products;
        return CORSHelper::jsonResponse(200, response.dump());
    });
}
