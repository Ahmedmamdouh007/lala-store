# Backend Setup and Integration Guide

## Current Status

✅ **Database**: Ready (13 products in `database/lala-store.db`)
✅ **Frontend Configuration**: Proxy correctly configured to `http://127.0.0.1:8001`
❌ **Backend**: Not running (needs to be built and started)

## Quick Start

### 1. Ensure Database is Ready
```bash
npm run ensure-db
```
This creates/updates the SQLite database with schema and seed data.

### 2. Build the Backend

**Windows (Visual Studio):**
```powershell
cd backend
cmake -B build -G "Visual Studio 17 2022"
cmake --build build --config Release
```

**Windows (MinGW/Ninja):**
```powershell
cd backend
cmake -B build -G "Ninja" -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++
cmake --build build --config Release
```

**Linux/Mac:**
```bash
cd backend
cmake -B build
cmake --build build --config Release
```

### 3. Start the Backend

**Windows:**
```bash
start_backend.bat
```

**Linux/Mac:**
```bash
./start_backend.sh
```

Or manually:
```bash
cd backend/build/Release  # or backend/build/
./lala_store  # or lala_store.exe on Windows
```

The backend will:
- Start on `http://localhost:8001`
- Automatically apply database schema if needed
- Create database file if missing

### 4. Verify Backend is Running

Run the integration test:
```bash
npm run test:integration
```

Or test manually:
```bash
curl http://localhost:8001/api/health
```

Expected response:
```json
{
  "ok": true,
  "database": true,
  "products_count": 13,
  "message": ""
}
```

### 5. Start the Frontend

In a new terminal:
```bash
cd frontend
npm install  # if not already done
npm run dev
```

Frontend will start on `http://localhost:3001` and proxy API requests to the backend.

## Testing the Full Stack

### Test Backend API Endpoints

```bash
# Health check
curl http://localhost:8001/api/health

# Get all products
curl http://localhost:8001/api/products

# Get men's products
curl http://localhost:8001/api/products/men

# Get women's products
curl http://localhost:8001/api/products/women

# Get featured products (home page)
curl http://localhost:8001/api/home/featured

# Get categories
curl http://localhost:8001/api/home/categories
```

### Test Frontend Pages

1. Open `http://localhost:3001` in your browser
2. Navigate to:
   - Home page (`/`) - should show featured products
   - Men's page (`/men`) - should show men's products
   - Women's page (`/women`) - should show women's products
   - Product details (`/product/:id`) - click any product card

### Verify Frontend → Backend Connection

1. Open browser DevTools (F12)
2. Go to Network tab
3. Navigate to Home page
4. Check for requests to `/api/home/featured` and `/api/home/categories`
5. Verify responses have `success: true` and `data` array

## Troubleshooting

### Backend Won't Build

**Error: "Generator Visual Studio 17 2022 could not find any instance"**
- Install Visual Studio 2022 with C++ workload, OR
- Install MinGW-w64 and use Ninja generator, OR
- Install Clang/LLVM

**Error: "CMake was unable to find a build program"**
- Install CMake: https://cmake.org/download/
- Ensure C++ compiler is in PATH

### Backend Won't Start

**Error: "Database connection failed"**
- Check `backend/config/db_config.json` exists
- Verify `database/lala-store.db` exists (run `npm run ensure-db`)
- Check file permissions

**Error: "Port 8001 already in use"**
- Stop other services using port 8001
- Or change port in `backend/main.cpp` (line 259)

### Frontend Shows "Backend offline" Notice

- Verify backend is running: `curl http://localhost:8001/api/health`
- Check frontend proxy config in `frontend/vite.config.js`
- Check browser console for CORS or network errors

### No Products Showing

1. Verify database has products:
   ```bash
   npm run ensure-db
   ```

2. Check backend health:
   ```bash
   curl http://localhost:8001/api/health
   ```
   Should show `products_count > 0`

3. Test API directly:
   ```bash
   curl http://localhost:8001/api/products
   ```

## Integration Test Script

Run comprehensive tests:
```bash
npm run test:integration
```

This tests:
- ✅ Database file exists
- ✅ Backend server is running
- ✅ API endpoints respond correctly
- ✅ Product details endpoint works
- ✅ Frontend proxy configuration

## Architecture Overview

```
┌─────────────┐         ┌──────────────┐         ┌─────────────┐
│   Browser   │ ──────> │   Frontend   │ ──────> │   Backend   │
│ localhost:  │         │ localhost:    │ Proxy   │ localhost:  │
│    3001     │         │    3001      │ /api/*  │    8001     │
└─────────────┘         └──────────────┘         └──────┬──────┘
                                                        │
                                                        ▼
                                                ┌──────────────┐
                                                │   SQLite DB  │
                                                │ lala-store.db│
                                                └──────────────┘
```

## API Endpoints Reference

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/api/health` | GET | Health check + database status |
| `/api/home/featured` | GET | Featured products for home page |
| `/api/home/categories` | GET | All categories |
| `/api/products` | GET | All products |
| `/api/products/men` | GET | Men's products |
| `/api/products/women` | GET | Women's products |
| `/api/products/details/:id` | GET | Product details by ID |
| `/api/cart/:userId` | GET | Get user's cart |
| `/api/cart/add` | POST | Add item to cart |
| `/api/cart/remove/:id` | DELETE | Remove item from cart |
| `/api/orders/create` | POST | Create order |
| `/api/orders/user/:id` | GET | Get user's orders |

## Next Steps

1. ✅ Database is ready
2. ⏳ Build backend (requires C++ toolchain)
3. ⏳ Start backend server
4. ⏳ Start frontend dev server
5. ⏳ Test full integration

Once backend is running, run `npm run test:integration` to verify everything works!
