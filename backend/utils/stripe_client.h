#ifndef STRIPE_CLIENT_H
#define STRIPE_CLIENT_H

#include <string>
#include <nlohmann/json.hpp>

namespace StripeClient {
    std::string getPublishableKey();
    std::string getWebhookSecret();
    // Initialize - load config. Call once at startup.
    void init();
    
    // Create PaymentIntent. amount_cents e.g. 1999 = $19.99
    // Returns json with client_secret, payment_intent_id or error
    nlohmann::json createPaymentIntent(int amount_cents, const std::string& currency = "usd");
    
    // Verify PaymentIntent with Stripe API. Returns json with status, ok
    nlohmann::json verifyPaymentIntent(const std::string& payment_intent_id);

    // Get Visa-safe payment details only (status, card_brand, card_last4). Never card number or CVV.
    nlohmann::json getPaymentIntentDetails(const std::string& payment_intent_id);
}

#endif
