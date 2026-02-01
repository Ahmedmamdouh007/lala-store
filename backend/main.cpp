#include <crow.h>
#include "db/connection.h"
#include "routes/home_routes.h"
#include "routes/product_routes.h"
#include "routes/cart_routes.h"
#include "routes/order_routes.h"
#include "utils/cors_helper.h"
#include <iostream>

int main() {
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
    
    // Setup routes
    setupHomeRoutes(app);
    setupProductRoutes(app);
    setupCartRoutes(app);
    setupOrderRoutes(app);
    
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
