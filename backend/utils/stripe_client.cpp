#include "stripe_client.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <curl/curl.h>
#include <cstring>

using json = nlohmann::json;

namespace {
    std::string stripeSecretKey;
    std::string stripePublishableKey;

    size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* out) {
        size_t total = size * nmemb;
        out->append((char*)contents, total);
        return total;
    }

    std::string urlEncode(const std::string& value) {
        std::string result;
        for (unsigned char c : value) {
            if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') result += c;
            else if (c == ' ') result += '+';
            else { char buf[4]; snprintf(buf, sizeof(buf), "%%%02X", c); result += buf; }
        }
        return result;
    }
}

namespace StripeClient {

void init() {
    stripeSecretKey = "sk_test_placeholder";
    std::ifstream f1("config/stripe_config.json");
    std::ifstream f2("../config/stripe_config.json");
    if (f1.is_open()) {
        try { json c; f1 >> c; if (c.contains("secret_key")) stripeSecretKey = c["secret_key"].get<std::string>(); if (c.contains("publishable_key")) stripePublishableKey = c["publishable_key"].get<std::string>(); } catch(...) {}
    } else if (f2.is_open()) {
        try { json c; f2 >> c; if (c.contains("secret_key")) stripeSecretKey = c["secret_key"].get<std::string>(); if (c.contains("publishable_key")) stripePublishableKey = c["publishable_key"].get<std::string>(); } catch(...) {}
    }
    const char* ek = getenv("STRIPE_SECRET_KEY");
    if (ek && strlen(ek) > 0) stripeSecretKey = ek;
    const char* pk = getenv("STRIPE_PUBLISHABLE_KEY");
    if (pk && strlen(pk) > 0) stripePublishableKey = pk;
}

std::string getPublishableKey() {
    return stripePublishableKey;
}

json createPaymentIntent(int amount_cents, const std::string& currency) {
    json result;
    if (stripeSecretKey.empty() || stripeSecretKey.find("YOUR_") != std::string::npos) {
        result["error"] = true;
        result["message"] = "Stripe not configured. Set STRIPE_SECRET_KEY or edit config/stripe_config.json";
        return result;
    }
    CURL* curl = curl_easy_init();
    if (!curl) { result["error"]=true; result["message"]="HTTP init failed"; return result; }
    std::string postData = "amount=" + std::to_string(amount_cents) + "&currency=" + urlEncode(currency) + "&payment_method_types[]=card";
    std::string responseBody;
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, ("Authorization: Bearer " + stripeSecretKey).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.stripe.com/v1/payment_intents");
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    if (res != CURLE_OK) {
        result["error"]=true; result["message"]=std::string("Stripe: ")+curl_easy_strerror(res);
        curl_easy_cleanup(curl);
        return result;
    }
    curl_easy_cleanup(curl);
    try {
        json sr = json::parse(responseBody);
        if (sr.contains("error")) {
            result["error"]=true; result["message"]=sr["error"].contains("message") ? sr["error"]["message"].get<std::string>() : "Stripe error";
            return result;
        }
        result["client_secret"] = sr["client_secret"]; result["payment_intent_id"] = sr["id"]; result["error"] = false;
    } catch (...) { result["error"]=true; result["message"]="Invalid Stripe response"; }
    return result;
}

json verifyPaymentIntent(const std::string& payment_intent_id) {
    json result;
    if (stripeSecretKey.empty() || stripeSecretKey.find("YOUR_") != std::string::npos) {
        result["ok"]=false; result["message"]="Stripe not configured"; return result;
    }
    CURL* curl = curl_easy_init();
    if (!curl) { result["ok"]=false; result["message"]="HTTP init failed"; return result; }
    std::string url = "https://api.stripe.com/v1/payment_intents/" + payment_intent_id;
    std::string responseBody;
    struct curl_slist* headers = curl_slist_append(nullptr, ("Authorization: Bearer " + stripeSecretKey).c_str());
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK) { result["ok"]=false; result["message"]=curl_easy_strerror(res); return result; }
    try {
        json sr = json::parse(responseBody);
        if (sr.contains("error")) { result["ok"]=false; result["status"]="error"; return result; }
        std::string status = sr["status"].get<std::string>();
        result["status"] = status; result["ok"] = (status == "succeeded");
    } catch (...) { result["ok"]=false; result["status"]="parse_error"; }
    return result;
}

}
