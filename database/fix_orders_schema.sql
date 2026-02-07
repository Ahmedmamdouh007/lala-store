-- Fix orders schema to match backend (shipping_address, product_id in order_items)
-- Run: psql -U postgres -d "Lala store" -f database/fix_orders_schema.sql

-- 1. Add missing columns to orders
ALTER TABLE orders ADD COLUMN IF NOT EXISTS shipping_address TEXT;
ALTER TABLE orders ADD COLUMN IF NOT EXISTS customer_name VARCHAR(150);
ALTER TABLE orders ADD COLUMN IF NOT EXISTS phone VARCHAR(30);
ALTER TABLE orders ADD COLUMN IF NOT EXISTS payment_method VARCHAR(50);

-- 2. Use total_amount (backend expects this)
DO $$
BEGIN
  IF EXISTS (SELECT 1 FROM information_schema.columns WHERE table_name='orders' AND column_name='total_price')
     AND NOT EXISTS (SELECT 1 FROM information_schema.columns WHERE table_name='orders' AND column_name='total_amount') THEN
    ALTER TABLE orders RENAME COLUMN total_price TO total_amount;
  ELSIF NOT EXISTS (SELECT 1 FROM information_schema.columns WHERE table_name='orders' AND column_name='total_amount') THEN
    ALTER TABLE orders ADD COLUMN total_amount DECIMAL(10,2);
  END IF;
END $$;

-- 4. Fix order_items: use product_id (cart uses products, not variants)
DROP TABLE IF EXISTS order_items CASCADE;

CREATE TABLE order_items (
    id SERIAL PRIMARY KEY,
    order_id INTEGER NOT NULL REFERENCES orders(id) ON DELETE CASCADE,
    product_id INTEGER NOT NULL REFERENCES products(id) ON DELETE CASCADE,
    quantity INTEGER NOT NULL CHECK (quantity > 0),
    price DECIMAL(10, 2) NOT NULL CHECK (price >= 0),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
CREATE INDEX IF NOT EXISTS idx_order_items_order ON order_items(order_id);
CREATE INDEX IF NOT EXISTS idx_order_items_product ON order_items(product_id);
