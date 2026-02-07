-- Fix cart schema to match backend (cart_items: user_id, product_id, quantity, price)
-- Run: psql -U postgres -d "Lala store" -f database/fix_cart_schema.sql

-- 1. Ensure we have a default user (id=1) for cart
INSERT INTO users (username, email, password_hash)
VALUES ('guest', 'guest@lalastore.com', '$2b$10$guest_hash')
ON CONFLICT (username) DO NOTHING;

-- If still no user, insert (some DBs may not have conflict)
INSERT INTO users (username, email, password_hash)
SELECT 'guest', 'guest@lalastore.com', '$2b$10$guest_hash'
WHERE NOT EXISTS (SELECT 1 FROM users WHERE username = 'guest');

-- 2. Drop old cart_items (different schema: cart_id, variant_id)
DROP TABLE IF EXISTS cart_items CASCADE;

-- 3. Create cart_items with schema backend expects
CREATE TABLE cart_items (
    id SERIAL PRIMARY KEY,
    user_id INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    product_id INTEGER NOT NULL REFERENCES products(id) ON DELETE CASCADE,
    quantity INTEGER NOT NULL DEFAULT 1 CHECK (quantity > 0),
    price DECIMAL(10, 2) NOT NULL CHECK (price >= 0),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    UNIQUE(user_id, product_id)
);

CREATE INDEX IF NOT EXISTS idx_cart_items_user ON cart_items(user_id);
CREATE INDEX IF NOT EXISTS idx_cart_items_product ON cart_items(product_id);
