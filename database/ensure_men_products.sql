-- Ensure database has sizes support and men's products
-- Run once: psql -U postgres -d "Lala store" -f database/ensure_men_products.sql

-- 1) Ensure size columns exist
ALTER TABLE products ADD COLUMN IF NOT EXISTS sizes TEXT;
ALTER TABLE products ADD COLUMN IF NOT EXISTS size_chart TEXT;
ALTER TABLE cart_items ADD COLUMN IF NOT EXISTS size VARCHAR(20) DEFAULT '';
ALTER TABLE cart_items DROP CONSTRAINT IF EXISTS cart_items_user_id_product_id_key;
DO $$ BEGIN
  IF NOT EXISTS (
    SELECT 1 FROM pg_constraint WHERE conname = 'cart_items_user_product_size_unique'
  ) THEN
    ALTER TABLE cart_items ADD CONSTRAINT cart_items_user_product_size_unique
      UNIQUE (user_id, product_id, size);
  END IF;
END $$;

-- 2) Insert men's products only if not already present (by name)
INSERT INTO products (name, description, price, image_url, category_id, gender, stock_quantity, sizes, size_chart)
SELECT v.name, v.description, v.price, v.image_url, v.category_id, v.gender, v.stock_quantity, v.sizes, v.size_chart
FROM (VALUES
  (
    'Mercedes-AMG Petronas Formula One Team White T-Shirt',
    'A classic white short-sleeve crew-neck t-shirt featuring the Mercedes-Benz star emblem alongside the "AMG PETRONAS FORMULA ONE TEAM" logo on the chest. Perfect for fans of motorsport and Mercedes-AMG Petronas. Made from lightweight cotton with a classic fit.',
    34.99,
    '/images/mercedes-amg-f1-tee.png',
    (SELECT id FROM categories WHERE name = 'T-Shirts' LIMIT 1),
    'men',
    50,
    'S,M,L,XL,XXL',
    'Chest:36,38,40,42,44 in;Length:27,28,29,30,31 in;Shoulder:17,18,19,20,21 in;Sleeve:7,7.5,8,8.5,9 in'
  ),
  (
    'Remember Who You Wanted To Be Black T-Shirt',
    'Black crew-neck t-shirt with inspirational quote: "REMEMBER WHO YOU WANTED TO BE" printed in dark grey and white. Simple, modern design with soft cotton fabric. Perfect for casual wear and motivational style.',
    28.99,
    '/images/remember-who-you-wanted-tee.png',
    (SELECT id FROM categories WHERE name = 'T-Shirts' LIMIT 1),
    'men',
    55,
    'S,M,L,XL,XXL',
    'Chest:36,38,40,42,44 in;Length:27,28,29,30,31 in;Shoulder:17,18,19,20,21 in;Sleeve:7,7.5,8,8.5,9 in'
  ),
  (
    'Abstract Wave Print Short Sleeve Button-Down Shirt',
    'Stylish short-sleeved men''s shirt featuring a unique black, grey, and white abstract wave pattern that sweeps diagonally across the front. Perfect for casual wear, vacations, or a modern statement look. Designed with a classic collar, full button placket, and a functional chest pocket. Comfortable and lightweight fabric for all-day wear.',
    49.99,
    '/images/abstract-black-grey-shirt.png',
    (SELECT id FROM categories WHERE name = 'T-Shirts' LIMIT 1),
    'men',
    30,
    'S,M,L,XL,XXL',
    'Chest:38,40,42,44,46 in;Length:29,30,31,32,33 in;Shoulder:18,19,20,21,22 in;Sleeve:6.5,7,7.5,8,8.5 in'
  ),
  (
    'Tropical Palm Tree Print Button-Up Shirt',
    'Dark grey short-sleeve shirt with a striking white vertical stripe featuring black palm tree silhouettes. Casual resort wear with a tropical aesthetic. Lightweight fabric, pointed collar, short cuffed sleeves, and chest pocket. Perfect for summer outings, beach environments, or vacation wear.',
    44.99,
    '/images/tropical-palm-shirt.png',
    (SELECT id FROM categories WHERE name = 'T-Shirts' LIMIT 1),
    'men',
    28,
    'S,M,L,XL,XXL',
    'Chest:38,40,42,44,46 in;Length:29,30,31,32,33 in;Shoulder:18,19,20,21,22 in;Sleeve:6.5,7,7.5,8,8.5 in'
  ),
  (
    'Diadora Abstract Graphic White T-Shirt',
    'White crew-neck t-shirt with bold abstract graphic design in teal, orange, and black. Features geometric swirl patterns and curvilinear shapes with high-contrast dot textures. Includes subtle Diadora branding. Modern, eye-catching design perfect for casual streetwear style.',
    36.99,
    '/images/diadora-abstract-tee.png',
    (SELECT id FROM categories WHERE name = 'T-Shirts' LIMIT 1),
    'men',
    42,
    'S,M,L,XL,XXL',
    'Chest:36,38,40,42,44 in;Length:27,28,29,30,31 in;Shoulder:17,18,19,20,21 in;Sleeve:7,7.5,8,8.5,9 in'
  )
) AS v(name, description, price, image_url, category_id, gender, stock_quantity, sizes, size_chart)
WHERE NOT EXISTS (SELECT 1 FROM products p WHERE p.name = v.name);
