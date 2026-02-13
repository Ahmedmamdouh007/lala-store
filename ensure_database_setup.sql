-- Ensure Database Setup for LALA STORE
-- Run this script to ensure the database has the required user and data

-- Ensure user with id=1 exists
INSERT INTO users (id, username, email, password_hash) 
VALUES (1, 'testuser', 'test@example.com', '$2b$10$example_hash_replace_in_production')
ON CONFLICT (id) DO NOTHING
ON CONFLICT (username) DO UPDATE SET id = 1 WHERE users.username = 'testuser';

-- If the above doesn't work, try this alternative:
-- First check if user exists, if not create it
DO $$
BEGIN
    IF NOT EXISTS (SELECT 1 FROM users WHERE id = 1) THEN
        INSERT INTO users (id, username, email, password_hash) 
        VALUES (1, 'testuser', 'test@example.com', '$2b$10$example_hash_replace_in_production');
    END IF;
END $$;

-- Verify user exists
SELECT id, username, email FROM users WHERE id = 1;

-- Verify products exist
SELECT COUNT(*) as product_count FROM products;

-- Verify categories exist  
SELECT COUNT(*) as category_count FROM categories;
