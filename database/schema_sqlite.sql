-- Lala Store - SQLite schema (file-based, no server)
-- Database file: lala-store.db (same app name)

PRAGMA foreign_keys = ON;

-- Categories
CREATE TABLE IF NOT EXISTS categories (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL UNIQUE,
    description TEXT,
    created_at TEXT DEFAULT (datetime('now'))
);

-- Users
CREATE TABLE IF NOT EXISTS users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT NOT NULL UNIQUE,
    email TEXT NOT NULL UNIQUE,
    password_hash TEXT NOT NULL,
    created_at TEXT DEFAULT (datetime('now'))
);

-- Products (with sizes for frontend)
CREATE TABLE IF NOT EXISTS products (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    description TEXT,
    price REAL NOT NULL CHECK (price >= 0),
    image_url TEXT,
    category_id INTEGER REFERENCES categories(id) ON DELETE SET NULL,
    gender TEXT NOT NULL CHECK (gender IN ('men', 'women', 'unisex')),
    stock_quantity INTEGER NOT NULL DEFAULT 0 CHECK (stock_quantity >= 0),
    sizes TEXT,
    size_chart TEXT,
    created_at TEXT DEFAULT (datetime('now'))
);

-- Cart items
CREATE TABLE IF NOT EXISTS cart_items (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    product_id INTEGER NOT NULL REFERENCES products(id) ON DELETE CASCADE,
    quantity INTEGER NOT NULL DEFAULT 1 CHECK (quantity > 0),
    price REAL NOT NULL CHECK (price >= 0),
    created_at TEXT DEFAULT (datetime('now')),
    UNIQUE(user_id, product_id)
);

-- Orders (with Stripe/PCI columns)
CREATE TABLE IF NOT EXISTS orders (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    total_amount REAL NOT NULL CHECK (total_amount >= 0),
    status TEXT NOT NULL DEFAULT 'pending' CHECK (status IN ('pending', 'paid', 'processing', 'shipped', 'delivered', 'cancelled', 'payment_failed')),
    shipping_address TEXT NOT NULL,
    customer_name TEXT,
    phone TEXT,
    payment_method TEXT,
    stripe_payment_intent_id TEXT UNIQUE,
    currency TEXT NOT NULL DEFAULT 'usd',
    card_brand TEXT,
    card_last4 TEXT,
    created_at TEXT DEFAULT (datetime('now'))
);

-- Order items
CREATE TABLE IF NOT EXISTS order_items (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    order_id INTEGER NOT NULL REFERENCES orders(id) ON DELETE CASCADE,
    product_id INTEGER NOT NULL REFERENCES products(id) ON DELETE CASCADE,
    quantity INTEGER NOT NULL CHECK (quantity > 0),
    price REAL NOT NULL CHECK (price >= 0),
    created_at TEXT DEFAULT (datetime('now'))
);

CREATE INDEX IF NOT EXISTS idx_products_category ON products(category_id);
CREATE INDEX IF NOT EXISTS idx_products_gender ON products(gender);
CREATE INDEX IF NOT EXISTS idx_cart_items_user ON cart_items(user_id);
CREATE INDEX IF NOT EXISTS idx_cart_items_product ON cart_items(product_id);
CREATE INDEX IF NOT EXISTS idx_orders_user ON orders(user_id);
CREATE INDEX IF NOT EXISTS idx_order_items_order ON order_items(order_id);
CREATE INDEX IF NOT EXISTS idx_orders_stripe_pi ON orders(stripe_payment_intent_id);

-- Seed categories
INSERT OR IGNORE INTO categories (id, name, description) VALUES
    (1, 'T-Shirts', 'Comfortable and stylish t-shirts'),
    (2, 'Jeans', 'Classic denim jeans'),
    (3, 'Dresses', 'Elegant dresses for every occasion'),
    (4, 'Jackets', 'Warm and fashionable jackets'),
    (5, 'Shoes', 'Comfortable footwear');

-- Seed default user (password: password123 - replace hash in production)
INSERT OR IGNORE INTO users (id, username, email, password_hash) VALUES
    (1, 'testuser', 'test@example.com', '$2b$10$example_hash_replace_in_production');

-- Seed products (only if table empty)
INSERT INTO products (name, description, price, image_url, category_id, gender, stock_quantity, sizes, size_chart)
SELECT 'Classic White T-Shirt', 'Comfortable cotton t-shirt for everyday wear', 19.99, 'https://via.placeholder.com/300x400?text=White+T-Shirt', 1, 'men', 50, NULL, NULL
WHERE (SELECT COUNT(*) FROM products) = 0
UNION ALL SELECT 'Blue Denim Jeans', 'Classic fit blue jeans', 49.99, 'https://via.placeholder.com/300x400?text=Blue+Jeans', 2, 'men', 30, NULL, NULL
WHERE (SELECT COUNT(*) FROM products) = 0
UNION ALL SELECT 'Summer Dress', 'Light and airy summer dress', 39.99, 'https://via.placeholder.com/300x400?text=Summer+Dress', 3, 'women', 25, NULL, NULL
WHERE (SELECT COUNT(*) FROM products) = 0
UNION ALL SELECT 'Leather Jacket', 'Stylish black leather jacket', 89.99, 'https://via.placeholder.com/300x400?text=Leather+Jacket', 4, 'men', 15, NULL, NULL
WHERE (SELECT COUNT(*) FROM products) = 0
UNION ALL SELECT 'Floral Print Dress', 'Beautiful floral print dress', 45.99, 'https://via.placeholder.com/300x400?text=Floral+Dress', 3, 'women', 20, NULL, NULL
WHERE (SELECT COUNT(*) FROM products) = 0
UNION ALL SELECT 'Sneakers', 'Comfortable running sneakers', 59.99, 'https://via.placeholder.com/300x400?text=Sneakers', 5, 'unisex', 40, NULL, NULL
WHERE (SELECT COUNT(*) FROM products) = 0
UNION ALL SELECT 'Casual Shirt', 'Button-down casual shirt', 34.99, 'https://via.placeholder.com/300x400?text=Casual+Shirt', 1, 'men', 35, NULL, NULL
WHERE (SELECT COUNT(*) FROM products) = 0
UNION ALL SELECT 'Maxi Dress', 'Elegant long maxi dress', 55.99, 'https://via.placeholder.com/300x400?text=Maxi+Dress', 3, 'women', 18, NULL, NULL
WHERE (SELECT COUNT(*) FROM products) = 0;
