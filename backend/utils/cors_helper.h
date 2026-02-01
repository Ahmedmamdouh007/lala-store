#ifndef CORS_HELPER_H
#define CORS_HELPER_H

#include <crow.h>

namespace CORSHelper {
    inline void addCORSHeaders(crow::response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.set_header("Content-Type", "application/json");
    }
    
    inline crow::response createCORSResponse(int code, const std::string& body) {
        crow::response res(code, body);
        addCORSHeaders(res);
        return res;
    }
    
    inline crow::response jsonResponse(int code, const std::string& jsonBody) {
        crow::response res(code, jsonBody);
        addCORSHeaders(res);
        return res;
    }
}

#endif // CORS_HELPER_H
