#ifndef RESPONSE_HELPER_H
#define RESPONSE_HELPER_H

#include <crow.h>
#include <string>
#include <vector>

namespace ResponseHelper {
    crow::response success(const std::string& message, const std::string& data = "{}");
    crow::response error(const std::string& message, int status_code = 400);
    crow::response jsonResponse(const std::string& json_data);
}

#endif // RESPONSE_HELPER_H
