/**
 * Fallback data when the backend API is unavailable.
 * Matches the seed data in database/schema_sqlite.sql so the home page
 * can show products even without the backend running.
 */

export const fallbackCategories = [
  { id: 1, name: 'T-Shirts', description: 'Comfortable and stylish t-shirts' },
  { id: 2, name: 'Jeans', description: 'Classic denim jeans' },
  { id: 3, name: 'Dresses', description: 'Elegant dresses for every occasion' },
  { id: 4, name: 'Jackets', description: 'Warm and fashionable jackets' },
  { id: 5, name: 'Shoes', description: 'Comfortable footwear' },
];

export const fallbackProducts = [
  { id: 1, name: 'Classic White T-Shirt', description: 'Comfortable cotton t-shirt for everyday wear', price: 19.99, image_url: 'https://via.placeholder.com/300x400?text=White+T-Shirt', category_id: 1, category_name: 'T-Shirts', gender: 'men', stock_quantity: 50, sizes: null, size_chart: null },
  { id: 2, name: 'Blue Denim Jeans', description: 'Classic fit blue jeans', price: 49.99, image_url: 'https://via.placeholder.com/300x400?text=Blue+Jeans', category_id: 2, category_name: 'Jeans', gender: 'men', stock_quantity: 30, sizes: null, size_chart: null },
  { id: 3, name: 'Summer Dress', description: 'Light and airy summer dress', price: 39.99, image_url: 'https://via.placeholder.com/300x400?text=Summer+Dress', category_id: 3, category_name: 'Dresses', gender: 'women', stock_quantity: 25, sizes: null, size_chart: null },
  { id: 4, name: 'Leather Jacket', description: 'Stylish black leather jacket', price: 89.99, image_url: 'https://via.placeholder.com/300x400?text=Leather+Jacket', category_id: 4, category_name: 'Jackets', gender: 'men', stock_quantity: 15, sizes: null, size_chart: null },
  { id: 5, name: 'Floral Print Dress', description: 'Beautiful floral print dress', price: 45.99, image_url: 'https://via.placeholder.com/300x400?text=Floral+Dress', category_id: 3, category_name: 'Dresses', gender: 'women', stock_quantity: 20, sizes: null, size_chart: null },
  { id: 6, name: 'Sneakers', description: 'Comfortable running sneakers', price: 59.99, image_url: 'https://via.placeholder.com/300x400?text=Sneakers', category_id: 5, category_name: 'Shoes', gender: 'unisex', stock_quantity: 40, sizes: null, size_chart: null },
  { id: 7, name: 'Casual Shirt', description: 'Button-down casual shirt', price: 34.99, image_url: 'https://via.placeholder.com/300x400?text=Casual+Shirt', category_id: 1, category_name: 'T-Shirts', gender: 'men', stock_quantity: 35, sizes: null, size_chart: null },
  { id: 8, name: 'Maxi Dress', description: 'Elegant long maxi dress', price: 55.99, image_url: 'https://via.placeholder.com/300x400?text=Maxi+Dress', category_id: 3, category_name: 'Dresses', gender: 'women', stock_quantity: 18, sizes: null, size_chart: null },
];

// Women's products (for Women page when backend is offline)
export const fallbackWomenProducts = [
  ...fallbackProducts.filter((p) => {
    const gender = (p.gender || '').toLowerCase();
    return gender === 'women' || gender === 'unisex';
  }),
];

// Men's products including the 5 new items (for Men page when backend is offline)
export const fallbackMenProducts = [
  ...fallbackProducts.filter((p) => (p.gender || '').toLowerCase() === 'men'),
  { id: 9, name: 'Diadora Geometric Print T-Shirt', description: "Men's white short-sleeve crew-neck t-shirt with a bold abstract geometric and serpentine graphic in teal, orange and black. Regular fit.", price: 29.99, image_url: '/images/men/diadora-geometric-tshirt.png', category_id: 1, category_name: 'T-Shirts', gender: 'men', stock_quantity: 40, sizes: null, size_chart: null },
  { id: 10, name: 'Mercedes-AMG Petronas F1 T-Shirt', description: 'White crew-neck t-shirt with official Mercedes-AMG Petronas Formula One Team branding: three-pointed star and AMG PETRONAS FORMULA ONE TEAM text.', price: 34.99, image_url: '/images/men/mercedes-amg-petronas-tshirt.png', category_id: 1, category_name: 'T-Shirts', gender: 'men', stock_quantity: 35, sizes: null, size_chart: null },
  { id: 11, name: 'Remember Who You Wanted To Be T-Shirt', description: 'Black short-sleeve t-shirt with motivational slogan in stacked grey and white text: REMEMBER WHO YOU WANTED TO BE. Soft fabric, round neck.', price: 24.99, image_url: '/images/men/remember-who-you-wanted-tshirt.png', category_id: 1, category_name: 'T-Shirts', gender: 'men', stock_quantity: 50, sizes: null, size_chart: null },
  { id: 12, name: 'Abstract Grayscale Button-Down Shirt', description: "Men's short-sleeve button-down with striking abstract wavy diagonal stripes in black, grey and white. Spread collar, chest pocket, relaxed fit.", price: 49.99, image_url: '/images/men/abstract-grayscale-shirt.png', category_id: 1, category_name: 'T-Shirts', gender: 'men', stock_quantity: 25, sizes: null, size_chart: null },
  { id: 13, name: 'Palm Tree Resort Shirt', description: 'Dark grey Hawaiian-style short-sleeve button-down with white vertical panel and black palm tree and bird silhouettes. Relaxed fit, ideal for summer.', price: 44.99, image_url: '/images/men/palm-tree-resort-shirt.png', category_id: 1, category_name: 'T-Shirts', gender: 'men', stock_quantity: 30, sizes: null, size_chart: null },
];
