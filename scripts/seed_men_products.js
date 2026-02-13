/**
 * Add the 5 men's products directly to the database.
 * Uses same config as backend: localhost, postgres, 1234, "Lala store"
 * Run from project root: node scripts/seed_men_products.js
 * Requires: npm install pg (or run from frontend after adding pg there)
 */

const fs = require('fs');
const path = require('path');

// Load backend config if available
let config = { host: 'localhost', port: 5432, database: 'Lala store', user: 'postgres', password: '1234' };
try {
  const cfgPath = path.join(__dirname, '..', 'backend', 'config', 'db_config.json');
  const data = fs.readFileSync(cfgPath, 'utf8');
  config = { ...config, ...JSON.parse(data) };
} catch (_) {}

async function run() {
  let client;
  try {
    const { Client } = require('pg');
    client = new Client({
      host: config.host,
      port: config.port || 5432,
      database: config.database,
      user: config.user,
      password: config.password,
    });
    await client.connect();
  } catch (e) {
    console.error('Cannot connect to PostgreSQL. Install pg: npm install pg');
    console.error(e.message);
    process.exit(1);
  }

  try {
    // Ensure columns exist
    await client.query('ALTER TABLE products ADD COLUMN IF NOT EXISTS sizes TEXT');
    await client.query('ALTER TABLE products ADD COLUMN IF NOT EXISTS size_chart TEXT');

    const categoryRes = await client.query("SELECT id FROM categories WHERE name = 'T-Shirts' LIMIT 1");
    const categoryId = categoryRes.rows[0]?.id ?? 1;

    const products = [
      {
        name: 'Mercedes-AMG Petronas Formula One Team White T-Shirt',
        description: 'A classic white short-sleeve crew-neck t-shirt featuring the Mercedes-Benz star emblem alongside the "AMG PETRONAS FORMULA ONE TEAM" logo on the chest. Perfect for fans of motorsport and Mercedes-AMG Petronas. Made from lightweight cotton with a classic fit.',
        price: 34.99,
        image_url: '/images/mercedes-amg-f1-tee.png',
        stock_quantity: 50,
        sizes: 'S,M,L,XL,XXL',
        size_chart: 'Chest:36,38,40,42,44 in;Length:27,28,29,30,31 in;Shoulder:17,18,19,20,21 in;Sleeve:7,7.5,8,8.5,9 in',
      },
      {
        name: 'Remember Who You Wanted To Be Black T-Shirt',
        description: 'Black crew-neck t-shirt with inspirational quote: "REMEMBER WHO YOU WANTED TO BE" printed in dark grey and white. Simple, modern design with soft cotton fabric. Perfect for casual wear and motivational style.',
        price: 28.99,
        image_url: '/images/remember-who-you-wanted-tee.png',
        stock_quantity: 55,
        sizes: 'S,M,L,XL,XXL',
        size_chart: 'Chest:36,38,40,42,44 in;Length:27,28,29,30,31 in;Shoulder:17,18,19,20,21 in;Sleeve:7,7.5,8,8.5,9 in',
      },
      {
        name: 'Abstract Wave Print Short Sleeve Button-Down Shirt',
        description: "Stylish short-sleeved men's shirt featuring a unique black, grey, and white abstract wave pattern that sweeps diagonally across the front. Perfect for casual wear, vacations, or a modern statement look. Designed with a classic collar, full button placket, and a functional chest pocket. Comfortable and lightweight fabric for all-day wear.",
        price: 49.99,
        image_url: '/images/abstract-black-grey-shirt.png',
        stock_quantity: 30,
        sizes: 'S,M,L,XL,XXL',
        size_chart: 'Chest:38,40,42,44,46 in;Length:29,30,31,32,33 in;Shoulder:18,19,20,21,22 in;Sleeve:6.5,7,7.5,8,8.5 in',
      },
      {
        name: 'Tropical Palm Tree Print Button-Up Shirt',
        description: 'Dark grey short-sleeve shirt with a striking white vertical stripe featuring black palm tree silhouettes. Casual resort wear with a tropical aesthetic. Lightweight fabric, pointed collar, short cuffed sleeves, and chest pocket. Perfect for summer outings, beach environments, or vacation wear.',
        price: 44.99,
        image_url: '/images/tropical-palm-shirt.png',
        stock_quantity: 28,
        sizes: 'S,M,L,XL,XXL',
        size_chart: 'Chest:38,40,42,44,46 in;Length:29,30,31,32,33 in;Shoulder:18,19,20,21,22 in;Sleeve:6.5,7,7.5,8,8.5 in',
      },
      {
        name: 'Diadora Abstract Graphic White T-Shirt',
        description: 'White crew-neck t-shirt with bold abstract graphic design in teal, orange, and black. Features geometric swirl patterns and curvilinear shapes with high-contrast dot textures. Includes subtle Diadora branding. Modern, eye-catching design perfect for casual streetwear style.',
        price: 36.99,
        image_url: '/images/diadora-abstract-tee.png',
        stock_quantity: 42,
        sizes: 'S,M,L,XL,XXL',
        size_chart: 'Chest:36,38,40,42,44 in;Length:27,28,29,30,31 in;Shoulder:17,18,19,20,21 in;Sleeve:7,7.5,8,8.5,9 in',
      },
    ];

    let inserted = 0;
    for (const p of products) {
      const exists = await client.query('SELECT 1 FROM products WHERE name = $1', [p.name]);
      if (exists.rows.length > 0) {
        console.log('Already exists:', p.name);
        continue;
      }
      await client.query(
        `INSERT INTO products (name, description, price, image_url, category_id, gender, stock_quantity, sizes, size_chart)
         VALUES ($1, $2, $3, $4, $5, 'men', $6, $7, $8)`,
        [p.name, p.description, p.price, p.image_url, categoryId, p.stock_quantity, p.sizes, p.size_chart]
      );
      console.log('Inserted:', p.name);
      inserted++;
    }

    console.log('\nDone. Inserted', inserted, 'product(s).');
  } finally {
    await client.end();
  }
}

run().catch((err) => {
  console.error(err);
  process.exit(1);
});
