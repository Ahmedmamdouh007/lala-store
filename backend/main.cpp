#include <crow.h>
#include <curl/curl.h>
#include "db/connection.h"
#include "routes/home_routes.h"
#include "routes/product_routes.h"
#include "routes/cart_routes.h"
#include "routes/order_routes.h"
#include "routes/stripe_routes.h"
#include "utils/cors_helper.h"
#include "utils/vulnerable_helper.h"
#include <iostream>
#include <sqlite3.h>
#include <nlohmann/json.hpp>
#include <cstring>
#include <cstdio>

int main() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    crow::SimpleApp app;
    
    // Handle OPTIONS requests for CORS preflight
    CROW_ROUTE(app, "/api/<path>")
    .methods("OPTIONS"_method)
    ([](const std::string&) {
        return CORSHelper::createCORSResponse(200, "");
    });
    
    CROW_ROUTE(app, "/")
    ([]() {
        return crow::response(200, "LALA STORE API Server is running!");
    });
    
    // --- VULNERABLE ROUTES (localhost lab only) ---
    // BUG-016 [Buffer overflow] strcpy into buf[32]
    CROW_ROUTE(app, "/api/lab/echo")
    .methods("POST"_method)
    ([](const crow::request& req) {
        char buf[32];
        buf[0] = '\0';
        try {
            auto body = nlohmann::json::parse(req.body);
            if (body.contains("input") && body["input"].is_string()) {
                std::string input = body["input"].get<std::string>();
                strcpy(buf, input.c_str());  // BUG-016
            }
        } catch (...) {}
        nlohmann::json j;
        j["echo"] = std::string(buf);
        return CORSHelper::jsonResponse(200, j.dump());
    });

    // BUG-017 [Buffer overflow] strcpy into name_buf[16]
    CROW_ROUTE(app, "/api/lab/user/<string>")
    ([](const std::string& username) {
        char name_buf[16];
        name_buf[0] = '\0';
        strcpy(name_buf, username.c_str());  // BUG-017
        nlohmann::json j;
        j["username"] = std::string(name_buf);
        return CORSHelper::jsonResponse(200, j.dump());
    });

    // BUG-018 [Buffer overflow] sprintf into out[24]
    CROW_ROUTE(app, "/api/lab/format")
    .methods("POST"_method)
    ([](const crow::request& req) {
        char out[24];
        try {
            auto body = nlohmann::json::parse(req.body);
            std::string msg = body.value("msg", "");
            std::string tag = body.value("tag", "");
            sprintf(out, "%s: %s", tag.c_str(), msg.c_str());  // BUG-018
        } catch (...) {
            sprintf(out, "error");
        }
        nlohmann::json j;
        j["formatted"] = std::string(out);
        return CORSHelper::jsonResponse(200, j.dump());
    });

    // Buffer overflow: request body copied into 64-byte stack buffer
    CROW_ROUTE(app, "/api/lab/comment")
    .methods("POST"_method)
    ([](const crow::request& req) {
        char comment[64];
        comment[0] = '\0';
        if (!req.body.empty()) {
            strncpy(comment, req.body.c_str(), sizeof(comment) - 1);
            comment[sizeof(comment)-1] = '\0';
        }
        nlohmann::json j;
        j["comment"] = std::string(comment);
        return CORSHelper::jsonResponse(200, j.dump());
    });
    // BUG-019 [Buffer overflow] strcpy into comment[64]
    CROW_ROUTE(app, "/api/lab/comment-raw")
    .methods("POST"_method)
    ([](const crow::request& req) {
        char comment[64];
        comment[0] = '\0';
        if (!req.body.empty())
            strcpy(comment, req.body.c_str());  // BUG-019
        nlohmann::json j;
        j["comment"] = std::string(comment);
        return CORSHelper::jsonResponse(200, j.dump());
    });

    // BUG-020 [Buffer overflow] copyToBuffer into buf[32] (see BUG-022)
    CROW_ROUTE(app, "/api/lab/copy")
    .methods("POST"_method)
    ([](const crow::request& req) {
        char buf[32];
        buf[0] = '\0';
        try {
            auto body = nlohmann::json::parse(req.body);
            std::string val = body.value("value", "");
            VulnerableHelper::copyToBuffer(val, buf);  // BUG-020
        } catch (...) {}
        nlohmann::json j;
        j["copied"] = std::string(buf);
        return CORSHelper::jsonResponse(200, j.dump());
    });

    // BUG-021 [Buffer overflow] formatGreeting into out[40] (see BUG-023)
    CROW_ROUTE(app, "/api/lab/greet")
    .methods("POST"_method)
    ([](const crow::request& req) {
        char out[40];
        out[0] = '\0';
        try {
            auto body = nlohmann::json::parse(req.body);
            std::string name = body.value("name", "Guest");
            VulnerableHelper::formatGreeting(name.c_str(), out);  // BUG-021
        } catch (...) {}
        nlohmann::json j;
        j["greeting"] = std::string(out);
        return CORSHelper::jsonResponse(200, j.dump());
    });

    // Health check: backend + database + product count (for troubleshooting)
    CROW_ROUTE(app, "/api/health")
    ([]() {
        nlohmann::json j;
        j["ok"] = true;
        j["database"] = false;
        j["products_count"] = 0;
        j["message"] = "";
        auto& db = DatabaseConnection::getInstance();
        if (!db.isConnected()) {
            j["database"] = false;
            j["message"] = "Database not connected. Check db_config.json and database/lala-store.db path.";
            return CORSHelper::jsonResponse(200, j.dump());
        }
        j["database"] = true;
        sqlite3* conn = db.getConnection();
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(conn, "SELECT COUNT(*) FROM products", -1, &stmt, nullptr) == SQLITE_OK && sqlite3_step(stmt) == SQLITE_ROW) {
            j["products_count"] = sqlite3_column_int(stmt, 0);
            sqlite3_finalize(stmt);
        } else {
            if (stmt) sqlite3_finalize(stmt);
            j["message"] = "Products table missing or error. Schema runs automatically on first start.";
        }
        return CORSHelper::jsonResponse(200, j.dump());
    });

    // Seed men's products (inserts 5 men's products if missing)
    CROW_ROUTE(app, "/api/seed/men-products")
    .methods("GET"_method)
    ([]() {
        auto& db = DatabaseConnection::getInstance();
        sqlite3* conn = db.getConnection();
        if (!db.isConnected()) {
            nlohmann::json e;
            e["success"] = false;
            e["message"] = "Database connection failed";
            return CORSHelper::jsonResponse(500, e.dump());
        }
        int categoryId = 1;
        sqlite3_stmt* catStmt = nullptr;
        if (sqlite3_prepare_v2(conn, "SELECT id FROM categories WHERE name = 'T-Shirts' LIMIT 1", -1, &catStmt, nullptr) == SQLITE_OK && sqlite3_step(catStmt) == SQLITE_ROW) {
            categoryId = sqlite3_column_int(catStmt, 0);
            sqlite3_finalize(catStmt);
        } else if (catStmt) sqlite3_finalize(catStmt);
        struct MenProduct { const char* name; const char* desc; double price; const char* img; int stock; const char* sizes; const char* chart; };
        MenProduct list[] = {
            { "Mercedes-AMG Petronas Formula One Team White T-Shirt",
              "A classic white short-sleeve crew-neck t-shirt featuring the Mercedes-Benz star emblem alongside the AMG PETRONAS FORMULA ONE TEAM logo on the chest. Perfect for fans of motorsport.",
              34.99, "/images/mercedes-amg-f1-tee.png", 50,
              "S,M,L,XL,XXL", "Chest:36,38,40,42,44 in;Length:27,28,29,30,31 in;Shoulder:17,18,19,20,21 in;Sleeve:7,7.5,8,8.5,9 in" },
            { "Remember Who You Wanted To Be Black T-Shirt",
              "Black crew-neck t-shirt with inspirational quote REMEMBER WHO YOU WANTED TO BE in dark grey and white. Simple, modern design with soft cotton fabric.",
              28.99, "/images/remember-who-you-wanted-tee.png", 55,
              "S,M,L,XL,XXL", "Chest:36,38,40,42,44 in;Length:27,28,29,30,31 in;Shoulder:17,18,19,20,21 in;Sleeve:7,7.5,8,8.5,9 in" },
            { "Abstract Wave Print Short Sleeve Button-Down Shirt",
              "Stylish short-sleeved men's shirt featuring a unique black, grey, and white abstract wave pattern. Perfect for casual wear and modern statement look.",
              49.99, "/images/abstract-black-grey-shirt.png", 30,
              "S,M,L,XL,XXL", "Chest:38,40,42,44,46 in;Length:29,30,31,32,33 in;Shoulder:18,19,20,21,22 in;Sleeve:6.5,7,7.5,8,8.5 in" },
            { "Tropical Palm Tree Print Button-Up Shirt",
              "Dark grey short-sleeve shirt with white vertical stripe and black palm tree silhouettes. Casual resort wear, lightweight fabric.",
              44.99, "/images/tropical-palm-shirt.png", 28,
              "S,M,L,XL,XXL", "Chest:38,40,42,44,46 in;Length:29,30,31,32,33 in;Shoulder:18,19,20,21,22 in;Sleeve:6.5,7,7.5,8,8.5 in" },
            { "Diadora Abstract Graphic White T-Shirt",
              "White crew-neck t-shirt with bold abstract graphic in teal, orange, and black. Diadora branding. Modern, eye-catching design for casual streetwear.",
              36.99, "/images/diadora-abstract-tee.png", 42,
              "S,M,L,XL,XXL", "Chest:36,38,40,42,44 in;Length:27,28,29,30,31 in;Shoulder:17,18,19,20,21 in;Sleeve:7,7.5,8,8.5,9 in" },
        };
        int inserted = 0;
        sqlite3_stmt* checkStmt = nullptr;
        sqlite3_prepare_v2(conn, "SELECT 1 FROM products WHERE name = ?1", -1, &checkStmt, nullptr);
        sqlite3_stmt* insStmt = nullptr;
        sqlite3_prepare_v2(conn, "INSERT INTO products (name, description, price, image_url, category_id, gender, stock_quantity, sizes, size_chart) VALUES (?1, ?2, ?3, ?4, ?5, 'men', ?6, ?7, ?8)", -1, &insStmt, nullptr);
        for (const auto& p : list) {
            if (checkStmt) {
                sqlite3_bind_text(checkStmt, 1, p.name, -1, SQLITE_STATIC);
                bool exists = (sqlite3_step(checkStmt) == SQLITE_ROW);
                sqlite3_reset(checkStmt);
                if (exists) continue;
            }
            if (insStmt) {
                sqlite3_bind_text(insStmt, 1, p.name, -1, SQLITE_STATIC);
                sqlite3_bind_text(insStmt, 2, p.desc, -1, SQLITE_STATIC);
                sqlite3_bind_double(insStmt, 3, p.price);
                sqlite3_bind_text(insStmt, 4, p.img, -1, SQLITE_STATIC);
                sqlite3_bind_int(insStmt, 5, categoryId);
                sqlite3_bind_int(insStmt, 6, p.stock);
                sqlite3_bind_text(insStmt, 7, p.sizes, -1, SQLITE_STATIC);
                sqlite3_bind_text(insStmt, 8, p.chart, -1, SQLITE_STATIC);
                if (sqlite3_step(insStmt) == SQLITE_DONE) inserted++;
                sqlite3_reset(insStmt);
            }
        }
        if (checkStmt) sqlite3_finalize(checkStmt);
        if (insStmt) sqlite3_finalize(insStmt);
        nlohmann::json resp;
        resp["success"] = true;
        resp["message"] = "Men's products seed completed.";
        resp["inserted"] = inserted;
        return CORSHelper::jsonResponse(200, resp.dump());
    });
    
    // Setup routes
    setupHomeRoutes(app);
    setupProductRoutes(app);
    setupCartRoutes(app);
    setupOrderRoutes(app);
    setupStripeRoutes(app);
    
    // Initialize database connection (creates lala-store.db from schema if missing)
    auto& db = DatabaseConnection::getInstance();
    if (!db.isConnected()) {
        std::cerr << "Warning: Database connection failed. Some features may not work." << std::endl;
    } else {
        db.ensureSchema();
    }
    
    std::cout << "Starting LALA STORE server on http://localhost:8001" << std::endl;
    std::cout << "API endpoints available at http://localhost:8001/api/" << std::endl;
    app.port(8001).multithreaded().run();
    
    return 0;
}
