# Visa / PCI-DSS Payment Standards (Lala Store)

This project implements **Visa-safe** card payments by following PCI-DSS best practices. The golden rule:

- **Your server must NEVER receive or store card number or CVV.**  
  If your backend touches raw card data, you become fully PCI-compliant (complex and expensive).

We avoid that by using a **PCI-compliant gateway (Stripe)** and only handling tokens and safe metadata.

---

## Architecture

| Layer | Responsibility |
|-------|----------------|
| **Frontend (React)** | Shows Stripe Elements; card data goes only to Stripe. Sends `payment_intent_id` and payment status to backend. |
| **Backend (C++ Crow)** | Creates PaymentIntent (server-to-server), verifies payment, saves order with **only** safe fields. Never sees card number or CVV. |
| **Stripe** | Handles card collection, authorization, and returns PaymentIntent ID and status. |

---

## What We Store (Visa-Safe Only)

Allowed in the database:

- `stripe_payment_intent_id` – reference to the payment
- `card_brand` – e.g. `visa`, `mastercard` (from Stripe API)
- `card_last4` – last 4 digits only (from Stripe API)
- `currency` – e.g. `usd`
- `status` – e.g. `pending`, `paid`, `payment_failed`

**Never stored:** card number, CVV, or any raw card data.

---

## Standards Checklist

1. **HTTPS everywhere**  
   Use TLS in production (e.g. Let’s Encrypt). Card forms must only be served over HTTPS.

2. **No storing CVV**  
   CVV is never stored, not even encrypted.

3. **Store only safe data**  
   As above: payment intent id, last4, brand, currency, status.

4. **Webhooks (recommended)**  
   Stripe can notify the backend:
   - `payment_intent.succeeded` → mark order as `paid`
   - `payment_intent.payment_failed` → mark order as `payment_failed`  
   Endpoint: `POST /api/stripe-webhook`. In production, verify the `Stripe-Signature` header using `webhook_secret` in `backend/config/stripe_config.json`.

---

## Flow Summary

1. **Frontend** asks backend for a PaymentIntent → **Backend** calls Stripe API → returns `client_secret` to frontend.
2. **Frontend** uses Stripe.js / Elements; customer enters card; Stripe returns success and `payment_intent_id`.
3. **Frontend** calls **Backend** to create order with `payment_intent_id` (no card data).
4. **Backend** verifies the PaymentIntent with Stripe, fetches safe details (last4, brand), then saves order with `stripe_payment_intent_id`, `card_brand`, `card_last4`, `currency`, `status`.
5. **Optional:** Stripe webhook sends `payment_intent.succeeded` / `payment_intent.payment_failed` → backend updates order status.

---

## Config

- **Stripe keys:** `backend/config/stripe_config.json` (or env `STRIPE_SECRET_KEY`, `STRIPE_PUBLISHABLE_KEY`).
- **Webhook secret:** `webhook_secret` in `stripe_config.json` (or `STRIPE_WEBHOOK_SECRET`) for signature verification in production.

---

## Database

Run the Visa/PCI migration so orders can store the safe fields above:

```bash
psql -U postgres -d "Lala store" -f database/visa_pci_orders_migration.sql
```

This adds `currency`, `card_brand`, `card_last4`, and allows statuses `paid` and `payment_failed`.
