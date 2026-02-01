#include "response_helper.h"
#include <crow.h>

namespace ResponseHelper {
    crow::response success(const std::string& message, const std::string& data) {
        crow::response res;
        res.code = 200;
        res.set_header("Content-Type", "application/json");
        res.body = "{\"success\":true,\"message\":\"" + message + "\",\"data\":" + data + "}";
        return res;
    }
    
    crow::response error(const std::string& message, int status_code) {
        crow::response res;
        res.code = status_code;
        res.set_header("Content-Type", "application/json");
        res.body = "{\"success\":false,\"message\":\"" + message + "\"}";
        return res;
    }
    
    crow::response jsonResponse(const std::string& json_data) {
        crow::response res;
        res.code = 200;
        res.set_header("Content-Type", "application/json");
        res.body = json_data;
        return res;
    }
}
