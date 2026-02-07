-- Add Stripe payment tracking to orders (PCI-safe: only store token/ID, never card data)
ALTER TABLE orders ADD COLUMN IF NOT EXISTS stripe_payment_intent_id TEXT;
