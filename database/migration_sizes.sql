-- Migration: Add sizes and size_chart support
-- Run: psql -U postgres -d "Lala store" -f database/migration_sizes.sql

-- Add size fields to products
ALTER TABLE products ADD COLUMN IF NOT EXISTS sizes TEXT;
ALTER TABLE products ADD COLUMN IF NOT EXISTS size_chart TEXT;

-- Add size to cart_items
ALTER TABLE cart_items ADD COLUMN IF NOT EXISTS size VARCHAR(20) DEFAULT '';

-- Update unique constraint to allow same product in different sizes
ALTER TABLE cart_items DROP CONSTRAINT IF EXISTS cart_items_user_id_product_id_key;
ALTER TABLE cart_items ADD CONSTRAINT cart_items_user_product_size_unique 
  UNIQUE (user_id, product_id, size);
