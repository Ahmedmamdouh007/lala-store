#include <crow.h>
#include "../utils/cors_helper.h"
#include "../utils/stripe_client.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void setupStripeRoutes(crow::SimpleApp& app) {
    StripeClient::init();
    
    CROW_ROUTE(app, "/api/create-payment-intent")
    .methods("POST"_method)
    ([](const crow::request& req) {
        try {
            auto body = json::parse(req.body);
            if (!body.contains("amount_cents")) {
                json resp; resp["success"]=false; resp["message"]="Missing amount_cents";
                return CORSHelper::jsonResponse(400, resp.dump());
            }
            int amount = body["amount_cents"].get<int>();
            if (amount < 50) {
                json resp; resp["success"]=false; resp["message"]="Amount must be at least 50 cents";
                return CORSHelper::jsonResponse(400, resp.dump());
            }
            std::string currency = body.value("currency", std::string("usd"));
            json result = StripeClient::createPaymentIntent(amount, currency);
            if (result.value("error", false)) {
                json resp; resp["success"]=false; resp["message"]=result.value("message","Stripe error");
                return CORSHelper::jsonResponse(500, resp.dump());
            }
            json resp;
            resp["success"] = true;
            resp["client_secret"] = result["client_secret"];
            resp["payment_intent_id"] = result["payment_intent_id"];
            return CORSHelper::jsonResponse(200, resp.dump());
        } catch (const std::exception& e) {
            json resp; resp["success"]=false; resp["message"]=std::string("Invalid request: ")+e.what();
            return CORSHelper::jsonResponse(400, resp.dump());
        }
    });
    
    CROW_ROUTE(app, "/api/stripe-config")
    ([]() {
        json resp;
        resp["publishableKey"] = StripeClient::getPublishableKey();
        return CORSHelper::jsonResponse(200, resp.dump());
    });

    CROW_ROUTE(app, "/api/verify-payment")
    .methods("POST"_method)
    ([](const crow::request& req) {
        try {
            auto body = json::parse(req.body);
            if (!body.contains("payment_intent_id")) {
                json resp; resp["success"]=false; resp["ok"]=false; resp["message"]="Missing payment_intent_id";
                return CORSHelper::jsonResponse(400, resp.dump());
            }
            std::string pi = body["payment_intent_id"].get<std::string>();
            json result = StripeClient::verifyPaymentIntent(pi);
            json resp;
            resp["success"] = true;
            resp["ok"] = result.value("ok", false);
            resp["status"] = result.value("status", "unknown");
            if (result.contains("message")) resp["message"] = result["message"];
            return CORSHelper::jsonResponse(200, resp.dump());
        } catch (const std::exception& e) {
            json resp; resp["success"]=false; resp["ok"]=false; resp["message"]=std::string("Invalid request: ")+e.what();
            return CORSHelper::jsonResponse(400, resp.dump());
        }
    });
}
