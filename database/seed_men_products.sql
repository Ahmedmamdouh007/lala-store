-- Seed: 7 men's products with photos, titles, descriptions, sizes, and size chart
-- Run migration_sizes.sql first, then this file.

-- Standard size chart for tops (inches)
-- Format: S|M|L|XL|XXL with Chest, Length, Shoulder, Sleeve
INSERT INTO products (name, description, price, image_url, category_id, gender, stock_quantity, sizes, size_chart) VALUES
(
  'Evangelion NERV Black T-Shirt',
  'Official-style Evangelion black tee featuring NERV logo, "01" and "TEST TYPE" graphics. Crew-neck, short-sleeve, smooth cotton blend. Perfect for anime fans.',
  32.99,
  '/images/evangelion-black-tee.png',
  (SELECT id FROM categories WHERE name = 'T-Shirts' LIMIT 1),
  'men',
  45,
  'S,M,L,XL,XXL',
  'Chest:36,38,40,42,44 in;Length:27,28,29,30,31 in;Shoulder:17,18,19,20,21 in;Sleeve:7,7.5,8,8.5,9 in'
),
(
  'Beige Good Times Take Time Graphic Tee',
  'Oversized beige cotton tee with black "GOOD TIMES TAKE TIME" typography and grayscale rose graphic on the back. Relaxed fit, dropped shoulders.',
  38.99,
  '/images/beige-good-times-tee.png',
  (SELECT id FROM categories WHERE name = 'T-Shirts' LIMIT 1),
  'men',
  35,
  'S,M,L,XL,XXL',
  'Chest:38,40,42,44,46 in;Length:28,29,30,31,32 in;Shoulder:18,19,20,21,22 in;Sleeve:8,8.5,9,9.5,10 in'
),
(
  'Mercedes-AMG Petronas F1 White T-Shirt',
  'White crew-neck tee with Mercedes-Benz star logo and AMG PETRONAS FORMULA ONE TEAM branding. Lightweight cotton, classic fit.',
  34.99,
  '/images/mercedes-amg-f1-tee.png',
  (SELECT id FROM categories WHERE name = 'T-Shirts' LIMIT 1),
  'men',
  50,
  'S,M,L,XL,XXL',
  'Chest:36,38,40,42,44 in;Length:27,28,29,30,31 in;Shoulder:17,18,19,20,21 in;Sleeve:7,7.5,8,8.5,9 in'
),
(
  'Abstract Black & Grey Button-Down Shirt',
  'Short-sleeve button-up with flowing abstract diagonal bands in black, grey, and white. Point collar, patch pocket. Futuristic, modern aesthetic.',
  49.99,
  '/images/abstract-black-grey-shirt.png',
  (SELECT id FROM categories WHERE name = 'T-Shirts' LIMIT 1),
  'men',
  30,
  'S,M,L,XL,XXL',
  'Chest:38,40,42,44,46 in;Length:29,30,31,32,33 in;Shoulder:18,19,20,21,22 in;Sleeve:6.5,7,7.5,8,8.5 in'
),
(
  'Diadora Abstract Graphic White T-Shirt',
  'White tee with bold abstract graphic in teal, orange, and black. Geometric swirl design, Diadora branding. Crew-neck, soft cotton.',
  36.99,
  '/images/diadora-abstract-tee.png',
  (SELECT id FROM categories WHERE name = 'T-Shirts' LIMIT 1),
  'men',
  42,
  'S,M,L,XL,XXL',
  'Chest:36,38,40,42,44 in;Length:27,28,29,30,31 in;Shoulder:17,18,19,20,21 in;Sleeve:7,7.5,8,8.5,9 in'
),
(
  'Tropical Palm Tree Print Button-Up Shirt',
  'Dark grey short-sleeve shirt with white vertical stripe and black palm tree silhouettes. Casual resort wear, lightweight fabric.',
  44.99,
  '/images/tropical-palm-shirt.png',
  (SELECT id FROM categories WHERE name = 'T-Shirts' LIMIT 1),
  'men',
  28,
  'S,M,L,XL,XXL',
  'Chest:38,40,42,44,46 in;Length:29,30,31,32,33 in;Shoulder:18,19,20,21,22 in;Sleeve:6.5,7,7.5,8,8.5 in'
),
(
  'Remember Who You Wanted To Be Black Tee',
  'Black crew-neck t-shirt with motivational text: "REMEMBER WHO YOU WANTED TO BE" in dark grey and white. Simple, modern, soft cotton.',
  28.99,
  '/images/remember-who-you-wanted-tee.png',
  (SELECT id FROM categories WHERE name = 'T-Shirts' LIMIT 1),
  'men',
  55,
  'S,M,L,XL,XXL',
  'Chest:36,38,40,42,44 in;Length:27,28,29,30,31 in;Shoulder:17,18,19,20,21 in;Sleeve:7,7.5,8,8.5,9 in'
);
