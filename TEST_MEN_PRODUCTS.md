# Men's Products – Database & Men's Page Test

## What’s in place

1. **Database**
   - `database/ensure_men_products.sql` – Ensures `sizes` / `size_chart` columns exist and inserts the 5 men’s products if they’re missing.
   - `database/add_mens_products.sql` – Inserts only the 5 products (run after migration/schema).

2. **Backend**
   - **Seed endpoint:** `GET http://localhost:8001/api/seed/men-products`  
     - Ensures `products.sizes` and `products.size_chart` exist.  
     - Inserts the 5 men’s products if they are not already in the DB (by name).  
     - Response: `{ "success": true, "message": "...", "inserted": 5 }`.

3. **Frontend**
   - Men’s page (`/men`) loads products from `GET /api/products/men`.  
   - No frontend changes required; once products are in the DB they show on the Men’s page.

4. **Test script**
   - `scripts/test_men_products_api.js`  
     - Calls the seed endpoint (so products are added if missing).  
     - Checks: `GET /api/products`, `GET /api/products/men`, presence of the 5 expected product names, `GET /api/home/featured`.  
   - Run: `node scripts/test_men_products_api.js` (backend must be running on port 8001).

## 1. Ensure products are in the database

**Option A – Use the API (recommended)**  
Backend must include the new seed route (see “Rebuild backend” below).

1. Start the backend (e.g. run `lala_store.exe` or your usual command).
2. In a browser or with curl:
   - `GET http://localhost:8001/api/seed/men-products`
3. You should see something like: `{ "success": true, "inserted": 5 }` (or `"inserted": 0` if they were already there).

**Option B – Use SQL (if you have `psql`)**  
From the project root:

```powershell
# If PostgreSQL is on PATH:
$env:PGPASSWORD = "1234"
psql -U postgres -d "Lala store" -f database/ensure_men_products.sql
```

Or run the PowerShell helper (it looks for `psql` in common install paths and on PATH):

```powershell
powershell -ExecutionPolicy Bypass -File add_mens_products.ps1
```

## 2. Rebuild backend (needed for the seed endpoint)

The seed route is in `backend/main.cpp`. Rebuild so the running server exposes `GET /api/seed/men-products`:

```powershell
cd backend
mkdir build -ErrorAction SilentlyContinue
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

Then run the backend from `backend/build/Release/lala_store.exe` (or your build output path).

## 3. Run the test

With the backend running on port 8001:

```powershell
node scripts/test_men_products_api.js
```

Expected:

- `[INFO] Seed men-products: 5 product(s) inserted.` (or 0 if already seeded).
- All `[PASS]` for GET /api/products, GET /api/products/men, expected product names, GET /api/home/featured.
- Final message: “All checks passed. Database and men's page API are working.”

## 4. Verify in the app

- Open the app (e.g. `http://localhost:3001`).
- Go to **Men’s** page – you should see the 5 products.
- Home page “Featured” may also show some of them (newest products).

## Troubleshooting

- **“Database connection failed”** – PostgreSQL running? Check `backend/config/db_config.json` and DB name `Lala store`.
- **“GET /api/seed/men-products” returns 404** – Rebuild the backend and restart the server so the new route is loaded.
- **Test script fails with “Missing men's products”** – Run the seed once (`GET /api/seed/men-products` or run the SQL), then run the test again.
- **Men’s page empty** – Ensure backend is running, then call `GET /api/products/men` (or open `/men`) and confirm the 5 products are returned.
