-- LALA STORE Database Schema
-- PostgreSQL Database: "Lala store"
-- User: postgres
-- Password: 1234

-- Create database (run this separately if database doesn't exist)
-- CREATE DATABASE "Lala store";

-- Enable UUID extension (optional, if needed)
-- CREATE EXTENSION IF NOT EXISTS "uuid-ossp";

-- Categories table
CREATE TABLE IF NOT EXISTS categories (
    id SERIAL PRIMARY KEY,
    name VARCHAR(100) NOT NULL UNIQUE,
    description TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Users table
CREATE TABLE IF NOT EXISTS users (
    id SERIAL PRIMARY KEY,
    username VARCHAR(50) NOT NULL UNIQUE,
    email VARCHAR(100) NOT NULL UNIQUE,
    password_hash VARCHAR(255) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Products table
CREATE TABLE IF NOT EXISTS products (
    id SERIAL PRIMARY KEY,
    name VARCHAR(200) NOT NULL,
    description TEXT,
    price DECIMAL(10, 2) NOT NULL CHECK (price >= 0),
    image_url VARCHAR(500),
    category_id INTEGER REFERENCES categories(id) ON DELETE SET NULL,
    gender VARCHAR(10) NOT NULL CHECK (gender IN ('men', 'women', 'unisex')),
    stock_quantity INTEGER NOT NULL DEFAULT 0 CHECK (stock_quantity >= 0),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Cart items table
CREATE TABLE IF NOT EXISTS cart_items (
    id SERIAL PRIMARY KEY,
    user_id INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    product_id INTEGER NOT NULL REFERENCES products(id) ON DELETE CASCADE,
    quantity INTEGER NOT NULL DEFAULT 1 CHECK (quantity > 0),
    price DECIMAL(10, 2) NOT NULL CHECK (price >= 0),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    UNIQUE(user_id, product_id)
);

-- Orders table
CREATE TABLE IF NOT EXISTS orders (
    id SERIAL PRIMARY KEY,
    user_id INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    total_amount DECIMAL(10, 2) NOT NULL CHECK (total_amount >= 0),
    status VARCHAR(20) NOT NULL DEFAULT 'pending' CHECK (status IN ('pending', 'processing', 'shipped', 'delivered', 'cancelled')),
    shipping_address TEXT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Order items table
CREATE TABLE IF NOT EXISTS order_items (
    id SERIAL PRIMARY KEY,
    order_id INTEGER NOT NULL REFERENCES orders(id) ON DELETE CASCADE,
    product_id INTEGER NOT NULL REFERENCES products(id) ON DELETE CASCADE,
    quantity INTEGER NOT NULL CHECK (quantity > 0),
    price DECIMAL(10, 2) NOT NULL CHECK (price >= 0),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Create indexes for better performance
CREATE INDEX IF NOT EXISTS idx_products_category ON products(category_id);
CREATE INDEX IF NOT EXISTS idx_products_gender ON products(gender);
CREATE INDEX IF NOT EXISTS idx_cart_items_user ON cart_items(user_id);
CREATE INDEX IF NOT EXISTS idx_cart_items_product ON cart_items(product_id);
CREATE INDEX IF NOT EXISTS idx_orders_user ON orders(user_id);
CREATE INDEX IF NOT EXISTS idx_order_items_order ON order_items(order_id);

-- Insert sample categories
INSERT INTO categories (name, description) VALUES
    ('T-Shirts', 'Comfortable and stylish t-shirts'),
    ('Jeans', 'Classic denim jeans'),
    ('Dresses', 'Elegant dresses for every occasion'),
    ('Jackets', 'Warm and fashionable jackets'),
    ('Shoes', 'Comfortable footwear')
ON CONFLICT (name) DO NOTHING;

-- Insert a default user (for testing)
-- Password: "password123" (hash this in production)
INSERT INTO users (username, email, password_hash) VALUES
    ('testuser', 'test@example.com', '$2b$10$example_hash_replace_in_production')
ON CONFLICT (username) DO NOTHING;

-- Insert sample products
INSERT INTO products (name, description, price, image_url, category_id, gender, stock_quantity) VALUES
    ('Classic White T-Shirt', 'Comfortable cotton t-shirt for everyday wear', 19.99, 'https://via.placeholder.com/300x400?text=White+T-Shirt', 1, 'men', 50),
    ('Blue Denim Jeans', 'Classic fit blue jeans', 49.99, 'https://via.placeholder.com/300x400?text=Blue+Jeans', 2, 'men', 30),
    ('Summer Dress', 'Light and airy summer dress', 39.99, 'https://via.placeholder.com/300x400?text=Summer+Dress', 3, 'women', 25),
    ('Leather Jacket', 'Stylish black leather jacket', 89.99, 'https://via.placeholder.com/300x400?text=Leather+Jacket', 4, 'men', 15),
    ('Floral Print Dress', 'Beautiful floral print dress', 45.99, 'https://via.placeholder.com/300x400?text=Floral+Dress', 3, 'women', 20),
    ('Sneakers', 'Comfortable running sneakers', 59.99, 'https://via.placeholder.com/300x400?text=Sneakers', 5, 'unisex', 40),
    ('Casual Shirt', 'Button-down casual shirt', 34.99, 'https://via.placeholder.com/300x400?text=Casual+Shirt', 1, 'men', 35),
    ('Maxi Dress', 'Elegant long maxi dress', 55.99, 'https://via.placeholder.com/300x400?text=Maxi+Dress', 3, 'women', 18)
ON CONFLICT DO NOTHING;
