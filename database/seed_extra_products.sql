-- Extra seed products (idempotent)
-- Adds basic men's t-shirts (black/white/blue) and women's dresses with real image URLs.

-- Men: Basic T-Shirts
INSERT INTO products (name, description, price, image_url, category_id, gender, stock_quantity)
SELECT
  'Minimal Black T-Shirt',
  'Clean fit black cotton tee — everyday essential.',
  24.99,
  'https://images.unsplash.com/photo-1499971442178-8c10fdf5f6ac?auto=format&fit=crop&w=1200&q=80',
  (SELECT id FROM categories WHERE name = 'T-Shirts' LIMIT 1),
  'men',
  80
WHERE NOT EXISTS (
  SELECT 1 FROM products WHERE name = 'Minimal Black T-Shirt' AND gender = 'men'
);

INSERT INTO products (name, description, price, image_url, category_id, gender, stock_quantity)
SELECT
  'Minimal White T-Shirt',
  'Crisp white tee with a simple, modern silhouette.',
  24.99,
  'https://images.unsplash.com/photo-1618677603286-0ec56cb6e1b5?auto=format&fit=crop&w=1200&q=80',
  (SELECT id FROM categories WHERE name = 'T-Shirts' LIMIT 1),
  'men',
  80
WHERE NOT EXISTS (
  SELECT 1 FROM products WHERE name = 'Minimal White T-Shirt' AND gender = 'men'
);

INSERT INTO products (name, description, price, image_url, category_id, gender, stock_quantity)
SELECT
  'Minimal Blue T-Shirt',
  'Soft blue tee — minimal look, easy layering.',
  26.99,
  'https://images.unsplash.com/photo-1579047917338-a6a69144fe63?auto=format&fit=crop&w=1200&q=80',
  (SELECT id FROM categories WHERE name = 'T-Shirts' LIMIT 1),
  'men',
  70
WHERE NOT EXISTS (
  SELECT 1 FROM products WHERE name = 'Minimal Blue T-Shirt' AND gender = 'men'
);

-- Women: Dresses
INSERT INTO products (name, description, price, image_url, category_id, gender, stock_quantity)
SELECT
  'Floral Summer Dress',
  'Lightweight floral dress designed for warm days.',
  59.99,
  'https://images.unsplash.com/photo-1759992878304-9470cac72939?auto=format&fit=crop&w=1200&q=80',
  (SELECT id FROM categories WHERE name = 'Dresses' LIMIT 1),
  'women',
  35
WHERE NOT EXISTS (
  SELECT 1 FROM products WHERE name = 'Floral Summer Dress' AND gender = 'women'
);

INSERT INTO products (name, description, price, image_url, category_id, gender, stock_quantity)
SELECT
  'Garden Floral Dress',
  'Simple floral dress with an elegant, natural vibe.',
  64.99,
  'https://images.unsplash.com/photo-1760172551779-5bd7f920a54b?auto=format&fit=crop&w=1200&q=80',
  (SELECT id FROM categories WHERE name = 'Dresses' LIMIT 1),
  'women',
  28
WHERE NOT EXISTS (
  SELECT 1 FROM products WHERE name = 'Garden Floral Dress' AND gender = 'women'
);

INSERT INTO products (name, description, price, image_url, category_id, gender, stock_quantity)
SELECT
  'White Beach Dress',
  'Airy white dress — minimal, soft, and timeless.',
  69.99,
  'https://images.unsplash.com/photo-1587798169572-8bde1b901d98?auto=format&fit=crop&w=1200&q=80',
  (SELECT id FROM categories WHERE name = 'Dresses' LIMIT 1),
  'women',
  22
WHERE NOT EXISTS (
  SELECT 1 FROM products WHERE name = 'White Beach Dress' AND gender = 'women'
);

