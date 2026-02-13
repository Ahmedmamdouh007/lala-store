-- Visa/PCI-safe orders: only store payment intent id, status, and safe card metadata (never card number or CVV)
-- Run: psql -U postgres -d "Lala store" -f database/visa_pci_orders_migration.sql

-- Stripe payment intent (already may exist from add_stripe_columns.sql)
ALTER TABLE orders ADD COLUMN IF NOT EXISTS stripe_payment_intent_id TEXT UNIQUE;

-- Currency (e.g. usd)
ALTER TABLE orders ADD COLUMN IF NOT EXISTS currency VARCHAR(10) NOT NULL DEFAULT 'usd';

-- Visa-safe metadata only (from Stripe API; never from frontend card input)
ALTER TABLE orders ADD COLUMN IF NOT EXISTS card_brand VARCHAR(20);
ALTER TABLE orders ADD COLUMN IF NOT EXISTS card_last4 VARCHAR(4);

-- Status: allow 'paid' and 'payment_failed' for Stripe flow
ALTER TABLE orders DROP CONSTRAINT IF EXISTS orders_status_check;
ALTER TABLE orders ADD CONSTRAINT orders_status_check CHECK (
  status IN ('pending', 'paid', 'processing', 'shipped', 'delivered', 'cancelled', 'payment_failed')
);

CREATE INDEX IF NOT EXISTS idx_orders_stripe_pi ON orders(stripe_payment_intent_id) WHERE stripe_payment_intent_id IS NOT NULL;
