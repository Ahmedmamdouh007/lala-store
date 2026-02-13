-- Seed products when table is empty (fix "no products")
-- Run: psql -U postgres -d "Lala store" -f database/seed_products_always.sql
-- Or from project root: psql -U postgres -d "Lala store" -f database/seed_products_always.sql

-- Ensure size columns exist (backend expects them)
ALTER TABLE products ADD COLUMN IF NOT EXISTS sizes TEXT;
ALTER TABLE products ADD COLUMN IF NOT EXISTS size_chart TEXT;

-- Ensure categories exist
INSERT INTO categories (name, description) VALUES
    ('T-Shirts', 'Comfortable and stylish t-shirts'),
    ('Jeans', 'Classic denim jeans'),
    ('Dresses', 'Elegant dresses for every occasion'),
    ('Jackets', 'Warm and fashionable jackets'),
    ('Shoes', 'Comfortable footwear')
ON CONFLICT (name) DO NOTHING;

-- Insert products only if table is empty
INSERT INTO products (name, description, price, image_url, category_id, gender, stock_quantity, sizes, size_chart)
SELECT name, description, price, image_url, category_id, gender, stock_quantity, sizes, size_chart
FROM (VALUES
    ('Classic White T-Shirt', 'Comfortable cotton t-shirt for everyday wear', 19.99::decimal, 'https://via.placeholder.com/300x400?text=White+T-Shirt', 1, 'men'::varchar, 50, NULL, NULL),
    ('Blue Denim Jeans', 'Classic fit blue jeans', 49.99::decimal, 'https://via.placeholder.com/300x400?text=Blue+Jeans', 2, 'men'::varchar, 30, NULL, NULL),
    ('Summer Dress', 'Light and airy summer dress', 39.99::decimal, 'https://via.placeholder.com/300x400?text=Summer+Dress', 3, 'women'::varchar, 25, NULL, NULL),
    ('Leather Jacket', 'Stylish black leather jacket', 89.99::decimal, 'https://via.placeholder.com/300x400?text=Leather+Jacket', 4, 'men'::varchar, 15, NULL, NULL),
    ('Floral Print Dress', 'Beautiful floral print dress', 45.99::decimal, 'https://via.placeholder.com/300x400?text=Floral+Dress', 3, 'women'::varchar, 20, NULL, NULL),
    ('Sneakers', 'Comfortable running sneakers', 59.99::decimal, 'https://via.placeholder.com/300x400?text=Sneakers', 5, 'unisex'::varchar, 40, NULL, NULL),
    ('Casual Shirt', 'Button-down casual shirt', 34.99::decimal, 'https://via.placeholder.com/300x400?text=Casual+Shirt', 1, 'men'::varchar, 35, NULL, NULL),
    ('Maxi Dress', 'Elegant long maxi dress', 55.99::decimal, 'https://via.placeholder.com/300x400?text=Maxi+Dress', 3, 'women'::varchar, 18, NULL, NULL)
) AS v(name, description, price, image_url, category_id, gender, stock_quantity, sizes, size_chart)
WHERE NOT EXISTS (SELECT 1 FROM products LIMIT 1);
