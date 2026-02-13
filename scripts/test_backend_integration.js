#!/usr/bin/env node
/**
 * Comprehensive test script to verify backend, database, and frontend integration.
 * Tests:
 * 1. Database exists and has data
 * 2. Backend server is running and responding
 * 3. All key API endpoints return correct data
 * 4. Frontend can connect to backend via proxy
 * 
 * Run: node scripts/test_backend_integration.js
 */

const fs = require('fs');
const path = require('path');
const http = require('http');

const BACKEND_URL = 'http://127.0.0.1:8001';
const API_BASE = `${BACKEND_URL}/api`;

const colors = {
  reset: '\x1b[0m',
  green: '\x1b[32m',
  red: '\x1b[31m',
  yellow: '\x1b[33m',
  blue: '\x1b[34m',
  cyan: '\x1b[36m',
};

function log(message, color = 'reset') {
  console.log(`${colors[color]}${message}${colors.reset}`);
}

function logSection(title) {
  console.log('\n' + '='.repeat(60));
  log(title, 'cyan');
  console.log('='.repeat(60));
}

function logSuccess(message) {
  log(`✓ ${message}`, 'green');
}

function logError(message) {
  log(`✗ ${message}`, 'red');
}

function logWarning(message) {
  log(`⚠ ${message}`, 'yellow');
}

function logInfo(message) {
  log(`ℹ ${message}`, 'blue');
}

// Test 1: Check database file exists
function testDatabaseFile() {
  logSection('Test 1: Database File Check');
  const projectRoot = path.resolve(__dirname, '..');
  const dbPath = path.join(projectRoot, 'database', 'lala-store.db');
  
  if (!fs.existsSync(dbPath)) {
    logError(`Database file not found: ${dbPath}`);
    logInfo('Run: npm run ensure-db');
    return false;
  }
  
  const stats = fs.statSync(dbPath);
  logSuccess(`Database file exists: ${dbPath}`);
  logInfo(`Size: ${(stats.size / 1024).toFixed(2)} KB`);
  return true;
}

// Test 2: Check backend server is running
function makeRequest(url, options = {}) {
  return new Promise((resolve, reject) => {
    const req = http.request(url, options, (res) => {
      let data = '';
      res.on('data', (chunk) => { data += chunk; });
      res.on('end', () => {
        try {
          const json = JSON.parse(data);
          resolve({ status: res.statusCode, data: json, raw: data });
        } catch {
          resolve({ status: res.statusCode, data: null, raw: data });
        }
      });
    });
    req.on('error', reject);
    req.setTimeout(5000, () => {
      req.destroy();
      reject(new Error('Request timeout'));
    });
    if (options.body) {
      req.write(options.body);
    }
    req.end();
  });
}

async function testBackendHealth() {
  logSection('Test 2: Backend Server Health Check');
  
  try {
    const response = await makeRequest(`${API_BASE}/health`);
    
    if (response.status === 200 && response.data) {
      logSuccess('Backend server is running');
      logInfo(`Database connected: ${response.data.database ? 'Yes' : 'No'}`);
      logInfo(`Products in database: ${response.data.products_count || 0}`);
      
      if (response.data.message) {
        logWarning(response.data.message);
      }
      
      if (!response.data.database) {
        logError('Database is not connected!');
        return false;
      }
      
      if (response.data.products_count === 0) {
        logWarning('No products found in database. Run: npm run ensure-db');
      }
      
      return true;
    } else {
      logError(`Unexpected response: ${response.status}`);
      return false;
    }
  } catch (error) {
    logError(`Backend server is not responding: ${error.message}`);
    logInfo('Make sure the backend is running:');
    logInfo('  Windows: start_backend.bat');
    logInfo('  Linux/Mac: ./start_backend.sh');
    logInfo('  Or build and run manually from backend/build/');
    return false;
  }
}

// Test 3: Test API endpoints
async function testAPIEndpoints() {
  logSection('Test 3: API Endpoints Check');
  
  const endpoints = [
    { name: 'Home - Featured Products', path: '/home/featured', method: 'GET' },
    { name: 'Home - Categories', path: '/home/categories', method: 'GET' },
    { name: 'All Products', path: '/products', method: 'GET' },
    { name: 'Men Products', path: '/products/men', method: 'GET' },
    { name: 'Women Products', path: '/products/women', method: 'GET' },
  ];
  
  let passed = 0;
  let failed = 0;
  
  for (const endpoint of endpoints) {
    try {
      const response = await makeRequest(`${API_BASE}${endpoint.path}`);
      
      if (response.status === 200 && response.data) {
        if (response.data.success !== false) {
          const data = response.data.data || response.data;
          const count = Array.isArray(data) ? data.length : (data ? 1 : 0);
          logSuccess(`${endpoint.name}: OK (${count} items)`);
          passed++;
        } else {
          logWarning(`${endpoint.name}: Response indicates failure`);
          logInfo(`Message: ${response.data.message || 'No message'}`);
          failed++;
        }
      } else {
        logError(`${endpoint.name}: HTTP ${response.status}`);
        failed++;
      }
    } catch (error) {
      logError(`${endpoint.name}: ${error.message}`);
      failed++;
    }
  }
  
  console.log(`\nResults: ${passed} passed, ${failed} failed`);
  return failed === 0;
}

// Test 4: Test product details endpoint
async function testProductDetails() {
  logSection('Test 4: Product Details Endpoint');
  
  try {
    // First get all products to find a valid ID
    const productsRes = await makeRequest(`${API_BASE}/products`);
    
    if (productsRes.status !== 200 || !productsRes.data?.success) {
      logWarning('Cannot test product details - no products available');
      return false;
    }
    
    const products = productsRes.data.data || [];
    if (products.length === 0) {
      logWarning('No products found to test details endpoint');
      return false;
    }
    
    const firstProduct = products[0];
    const productId = firstProduct.id;
    
    logInfo(`Testing with product ID: ${productId} (${firstProduct.name})`);
    
    const detailsRes = await makeRequest(`${API_BASE}/products/details/${productId}`);
    
    if (detailsRes.status === 200 && detailsRes.data?.success) {
      const product = detailsRes.data.data;
      logSuccess(`Product details endpoint works`);
      logInfo(`Product: ${product.name} - $${product.price}`);
      return true;
    } else {
      logError(`Product details endpoint failed: ${detailsRes.status}`);
      return false;
    }
  } catch (error) {
    logError(`Product details test failed: ${error.message}`);
    return false;
  }
}

// Test 5: Verify frontend proxy configuration
function testFrontendConfig() {
  logSection('Test 5: Frontend Configuration Check');
  
  const projectRoot = path.resolve(__dirname, '..');
  const viteConfigPath = path.join(projectRoot, 'frontend', 'vite.config.js');
  
  if (!fs.existsSync(viteConfigPath)) {
    logError('vite.config.js not found');
    return false;
  }
  
  const viteConfig = fs.readFileSync(viteConfigPath, 'utf8');
  
  if (viteConfig.includes('127.0.0.1:8001') || viteConfig.includes('localhost:8001')) {
    logSuccess('Frontend proxy configured correctly');
    logInfo('Proxy target: http://127.0.0.1:8001');
    return true;
  } else {
    logWarning('Frontend proxy configuration may be incorrect');
    return false;
  }
}

// Main test runner
async function main() {
  console.log('\n');
  log('='.repeat(60), 'cyan');
  log('LALA STORE - Backend Integration Test', 'cyan');
  log('='.repeat(60), 'cyan');
  
  const results = {
    databaseFile: testDatabaseFile(),
    backendHealth: await testBackendHealth(),
    apiEndpoints: await testAPIEndpoints(),
    productDetails: await testProductDetails(),
    frontendConfig: testFrontendConfig(),
  };
  
  logSection('Test Summary');
  
  const allPassed = Object.values(results).every(r => r === true);
  const passedCount = Object.values(results).filter(r => r === true).length;
  const totalCount = Object.keys(results).length;
  
  console.log(`\nTests passed: ${passedCount}/${totalCount}`);
  
  if (allPassed) {
    log('\n✓ All tests passed! Backend, database, and frontend are properly configured.', 'green');
    console.log('\nNext steps:');
    logInfo('1. Start backend: start_backend.bat (Windows) or ./start_backend.sh (Linux/Mac)');
    logInfo('2. Start frontend: cd frontend && npm run dev');
    logInfo('3. Open browser: http://localhost:3001');
  } else {
    log('\n✗ Some tests failed. Please fix the issues above.', 'red');
    console.log('\nTroubleshooting:');
    
    if (!results.databaseFile) {
      logInfo('1. Setup database: npm run ensure-db');
    }
    if (!results.backendHealth) {
      logInfo('2. Build backend: cd backend && cmake -B build && cmake --build build --config Release');
      logInfo('3. Start backend: start_backend.bat or ./start_backend.sh');
    }
    if (!results.apiEndpoints || !results.productDetails) {
      logInfo('4. Ensure database has products: npm run ensure-db');
    }
  }
  
  console.log('');
  process.exit(allPassed ? 0 : 1);
}

main().catch((error) => {
  logError(`Test runner error: ${error.message}`);
  console.error(error);
  process.exit(1);
});
