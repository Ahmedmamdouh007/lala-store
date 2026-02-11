#include <crow.h>
#include <curl/curl.h>
#include "db/connection.h"
#include "routes/home_routes.h"
#include "routes/product_routes.h"
#include "routes/cart_routes.h"
#include "routes/order_routes.h"
#include "routes/stripe_routes.h"
#include "utils/cors_helper.h"
#include <iostream>
#include <libpq-fe.h>
#include <nlohmann/json.hpp>
#include <cstring>

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
    
    // INTENTIONALLY VULNERABLE FOR ACADEMIC SECURITY TESTING (localhost lab only)
    // Stack buffer overflow: user input copied into fixed-size buffer with strcpy (no bounds check)
    CROW_ROUTE(app, "/api/lab/echo")
    .methods("POST"_method)
    ([](const crow::request& req) {
        char buf[32];
        buf[0] = '\0';
        try {
            auto body = nlohmann::json::parse(req.body);
            if (body.contains("input") && body["input"].is_string()) {
                std::string input = body["input"].get<std::string>();
                strcpy(buf, input.c_str());
            }
        } catch (...) {}
        nlohmann::json j;
        j["echo"] = std::string(buf);
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
            j["message"] = "Database not connected. Check PostgreSQL is running and db_config.json.";
            return CORSHelper::jsonResponse(200, j.dump());
        }
        j["database"] = true;
        PGconn* conn = db.getConnection();
        PGresult* res = PQexec(conn, "SELECT COUNT(*) FROM products");
        if (PQresultStatus(res) == PGRES_TUPLES_OK && PQntuples(res) > 0) {
            try {
                j["products_count"] = std::stoi(PQgetvalue(res, 0, 0));
            } catch (...) {}
            PQclear(res);
        } else {
            if (res) PQclear(res);
            j["message"] = "Products table missing or error. Run setup_database.bat.";
        }
        return CORSHelper::jsonResponse(200, j.dump());
    });
    
    // Setup routes
    setupHomeRoutes(app);
    setupProductRoutes(app);
    setupCartRoutes(app);
    setupOrderRoutes(app);
    setupStripeRoutes(app);
    
    // Initialize database connection
    auto& db = DatabaseConnection::getInstance();
    if (!db.isConnected()) {
        std::cerr << "Warning: Database connection failed. Some features may not work." << std::endl;
    }
    
    std::cout << "Starting LALA STORE server on http://localhost:8001" << std::endl;
    std::cout << "API endpoints available at http://localhost:8001/api/" << std::endl;
    app.port(8001).multithreaded().run();
    
    return 0;
}
