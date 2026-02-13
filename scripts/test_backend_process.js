#!/usr/bin/env node
/**
 * Test backend process: connection, database, and all flows the frontend uses.
 * Run: node scripts/test_backend_process.js
 */

const http = require('http');

const API = 'http://127.0.0.1:8001/api';

function get(path) {
  return new Promise((resolve, reject) => {
    const url = path.startsWith('http') ? path : `${API}${path}`;
    const req = http.request(url, (res) => {
      let data = '';
      res.on('data', (c) => { data += c; });
      res.on('end', () => {
        try {
          resolve({ status: res.statusCode, data: JSON.parse(data) });
        } catch {
          resolve({ status: res.statusCode, data: null, raw: data });
        }
      });
    });
    req.on('error', reject);
    req.setTimeout(5000, () => { req.destroy(); reject(new Error('timeout')); });
    req.end();
  });
}

async function main() {
  console.log('\n--- Backend process test (connection + database + API flows) ---\n');

  let failed = 0;

  // 1. Health + DB connection
  try {
    const h = await get('/health');
    if (h.status !== 200 || !h.data) throw new Error('Invalid health response');
    if (!h.data.ok) throw new Error('Health ok=false');
    if (!h.data.database) throw new Error('Database not connected');
    console.log('[PASS] Health: backend OK, database connected');
    console.log(`       Products in DB: ${h.data.products_count ?? 0}`);
  } catch (e) {
    console.log('[FAIL] Health / DB:', e.message);
    failed++;
    console.log('\nStart backend first: start_backend_mingw.bat (or start_backend.bat)\n');
    process.exit(1);
  }

  // 2. Home flow (featured + categories)
  try {
    const [feat, cat] = await Promise.all([get('/home/featured'), get('/home/categories')]);
    if (feat.status !== 200 || !feat.data?.success) throw new Error('Featured failed');
    if (cat.status !== 200 || !cat.data?.success) throw new Error('Categories failed');
    const fCount = Array.isArray(feat.data?.data) ? feat.data.data.length : 0;
    const cCount = Array.isArray(cat.data?.data) ? cat.data.data.length : 0;
    console.log('[PASS] Home flow: featured=' + fCount + ', categories=' + cCount);
  } catch (e) {
    console.log('[FAIL] Home flow:', e.message);
    failed++;
  }

  // 3. Products list (all, men, women)
  try {
    const [all, men, women] = await Promise.all([
      get('/products'),
      get('/products/men'),
      get('/products/women'),
    ]);
    if (all.status !== 200 || !all.data?.success) throw new Error('GET /products failed');
    if (men.status !== 200 || !men.data?.success) throw new Error('GET /products/men failed');
    if (women.status !== 200 || !women.data?.success) throw new Error('GET /products/women failed');
    const a = all.data.data?.length ?? 0;
    const m = men.data.data?.length ?? 0;
    const w = women.data.data?.length ?? 0;
    console.log('[PASS] Products: all=' + a + ', men=' + m + ', women=' + w);
  } catch (e) {
    console.log('[FAIL] Products list:', e.message);
    failed++;
  }

  // 4. Product details for every product (no "Product not found")
  try {
    const all = await get('/products');
    const list = all.data?.data ?? [];
    let detailsOk = 0;
    let detailsFail = 0;
    for (const p of list) {
      const d = await get('/products/details/' + p.id);
      if (d.status === 200 && d.data?.success && d.data?.data) detailsOk++;
      else detailsFail++;
    }
    if (detailsFail > 0) {
      console.log('[FAIL] Product details: ' + detailsOk + ' OK, ' + detailsFail + ' failed (Product not found)');
      failed++;
    } else {
      console.log('[PASS] Product details: all ' + detailsOk + ' products return valid details');
    }
  } catch (e) {
    console.log('[FAIL] Product details flow:', e.message);
    failed++;
  }

  // 5. Cart read (backend process: read cart for user 1)
  try {
    const cart = await get('/cart/1');
    if (cart.status !== 200) throw new Error('Cart returned ' + cart.status);
    const items = cart.data?.data ?? [];
    console.log('[PASS] Cart API: GET /cart/1 OK (items: ' + items.length + ')');
  } catch (e) {
    console.log('[FAIL] Cart API:', e.message);
    failed++;
  }

  // 6. Stripe config (backend process: config endpoint)
  try {
    const stripe = await get('/stripe-config');
    if (stripe.status !== 200) throw new Error('Stripe config returned ' + stripe.status);
    console.log('[PASS] Stripe config: endpoint responds');
  } catch (e) {
    console.log('[WARN] Stripe config:', e.message);
  }

  console.log('');
  if (failed === 0) {
    console.log('Backend process test: all checks passed. Backend is running correctly and connected to DB and APIs.');
  } else {
    console.log('Backend process test: ' + failed + ' check(s) failed.');
  }
  console.log('');
  process.exit(failed > 0 ? 1 : 0);
}

main().catch((err) => {
  console.error('Error:', err.message);
  process.exit(1);
});
