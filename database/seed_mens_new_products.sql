-- Add 5 new men's products (images in frontend/public/images/men/)
-- Safe to run multiple times: only inserts if name does not already exist.

INSERT INTO products (name, description, price, image_url, category_id, gender, stock_quantity, sizes, size_chart)
SELECT 'Diadora Geometric Print T-Shirt', 'Men''s white short-sleeve crew-neck t-shirt with a bold abstract geometric and serpentine graphic in teal, orange and black. Regular fit.', 29.99, '/images/men/diadora-geometric-tshirt.png', 1, 'men', 40, NULL, NULL
WHERE NOT EXISTS (SELECT 1 FROM products WHERE name = 'Diadora Geometric Print T-Shirt');

INSERT INTO products (name, description, price, image_url, category_id, gender, stock_quantity, sizes, size_chart)
SELECT 'Mercedes-AMG Petronas F1 T-Shirt', 'White crew-neck t-shirt with official Mercedes-AMG Petronas Formula One Team branding: three-pointed star and AMG PETRONAS FORMULA ONE TEAM text.', 34.99, '/images/men/mercedes-amg-petronas-tshirt.png', 1, 'men', 35, NULL, NULL
WHERE NOT EXISTS (SELECT 1 FROM products WHERE name = 'Mercedes-AMG Petronas F1 T-Shirt');

INSERT INTO products (name, description, price, image_url, category_id, gender, stock_quantity, sizes, size_chart)
SELECT 'Remember Who You Wanted To Be T-Shirt', 'Black short-sleeve t-shirt with motivational slogan in stacked grey and white text: REMEMBER WHO YOU WANTED TO BE. Soft fabric, round neck.', 24.99, '/images/men/remember-who-you-wanted-tshirt.png', 1, 'men', 50, NULL, NULL
WHERE NOT EXISTS (SELECT 1 FROM products WHERE name = 'Remember Who You Wanted To Be T-Shirt');

INSERT INTO products (name, description, price, image_url, category_id, gender, stock_quantity, sizes, size_chart)
SELECT 'Abstract Grayscale Button-Down Shirt', 'Men''s short-sleeve button-down with striking abstract wavy diagonal stripes in black, grey and white. Spread collar, chest pocket, relaxed fit.', 49.99, '/images/men/abstract-grayscale-shirt.png', 1, 'men', 25, NULL, NULL
WHERE NOT EXISTS (SELECT 1 FROM products WHERE name = 'Abstract Grayscale Button-Down Shirt');

INSERT INTO products (name, description, price, image_url, category_id, gender, stock_quantity, sizes, size_chart)
SELECT 'Palm Tree Resort Shirt', 'Dark grey Hawaiian-style short-sleeve button-down with white vertical panel and black palm tree and bird silhouettes. Relaxed fit, ideal for summer.', 44.99, '/images/men/palm-tree-resort-shirt.png', 1, 'men', 30, NULL, NULL
WHERE NOT EXISTS (SELECT 1 FROM products WHERE name = 'Palm Tree Resort Shirt');
