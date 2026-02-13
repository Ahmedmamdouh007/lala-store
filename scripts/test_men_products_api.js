/**
 * Test that men's products are in the database and returned by the API.
 * Requires: backend running on http://localhost:8001
 * Run: node scripts/test_men_products_api.js
 */

const API_BASE = 'http://localhost:8001/api';
const EXPECTED_MEN_PRODUCTS = [
  'Mercedes-AMG Petronas Formula One Team White T-Shirt',
  'Remember Who You Wanted To Be Black T-Shirt',
  'Abstract Wave Print Short Sleeve Button-Down Shirt',
  'Tropical Palm Tree Print Button-Up Shirt',
  'Diadora Abstract Graphic White T-Shirt',
];

async function get(url) {
  const res = await fetch(url);
  const text = await res.text();
  if (!res.ok) throw new Error(`HTTP ${res.status}: ${text}`);
  return JSON.parse(text);
}

async function main() {
  console.log('Testing LALA STORE API (men products & database)...\n');

  let passed = 0;
  let failed = 0;

  // 0) Seed men's products if missing
  try {
    const seedRes = await get(`${API_BASE}/seed/men-products`);
    if (seedRes.success && seedRes.inserted !== undefined) {
      console.log(`  [INFO] Seed men-products: ${seedRes.inserted} product(s) inserted.`);
    }
  } catch (e) {
    console.log(`  [WARN] Seed endpoint: ${e.message}`);
  }

  // 1) GET /api/products
  try {
    const allRes = await get(`${API_BASE}/products`);
    if (!allRes.success || !Array.isArray(allRes.data)) {
      throw new Error('Expected { success: true, data: [...] }');
    }
    console.log(`  [PASS] GET /api/products returned ${allRes.data.length} product(s)`);
    passed++;
  } catch (e) {
    console.log(`  [FAIL] GET /api/products: ${e.message}`);
    failed++;
  }

  // 2) GET /api/products/men
  let menProducts = [];
  try {
    const menRes = await get(`${API_BASE}/products/men`);
    if (!menRes.success || !Array.isArray(menRes.data)) {
      throw new Error('Expected { success: true, data: [...] }');
    }
    menProducts = menRes.data;
    console.log(`  [PASS] GET /api/products/men returned ${menProducts.length} men's product(s)`);
    passed++;
  } catch (e) {
    console.log(`  [FAIL] GET /api/products/men: ${e.message}`);
    failed++;
  }

  // 3) Check expected men's product names exist
  const names = (menProducts || []).map((p) => p.name || '');
  const missing = EXPECTED_MEN_PRODUCTS.filter((n) => !names.includes(n));
  if (missing.length === 0 && EXPECTED_MEN_PRODUCTS.length > 0) {
    console.log(`  [PASS] All ${EXPECTED_MEN_PRODUCTS.length} expected men's products found on API`);
    passed++;
  } else if (missing.length > 0) {
    console.log(`  [FAIL] Missing men's products: ${missing.join(', ')}`);
    failed++;
  }

  // 4) GET /api/home/featured (home page)
  try {
    const featuredRes = await get(`${API_BASE}/home/featured`);
    if (!featuredRes.success || !Array.isArray(featuredRes.data)) {
      throw new Error('Expected { success: true, data: [...] }');
    }
    console.log(`  [PASS] GET /api/home/featured returned ${featuredRes.data.length} product(s)`);
    passed++;
  } catch (e) {
    console.log(`  [FAIL] GET /api/home/featured: ${e.message}`);
    failed++;
  }

  console.log('');
  if (failed === 0) {
    console.log('All checks passed. Database and men\'s page API are working.');
  } else {
    console.log(`${passed} passed, ${failed} failed.`);
  }
  process.exit(failed === 0 ? 0 : 1);
}

main().catch((err) => {
  console.error('Error:', err.message);
  console.error('Make sure the backend is running: http://localhost:8001');
  process.exit(1);
});
