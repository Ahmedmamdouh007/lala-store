# Fix "No Products" Showing

If the store shows no products, use one of these:

## Option 1: Seed via API (easiest)

1. Start the **backend** (port 8001) and ensure PostgreSQL is running.
2. Open in browser or with curl:
   ```
   http://localhost:8001/api/seed/men-products
   ```
3. This adds 5 men's products (and ensures `sizes` / `size_chart` columns exist). Refresh the store.

## Option 2: Run SQL seed script

From the project root (adjust `postgres` / password if needed):

**Windows (PowerShell):**
```powershell
psql -U postgres -d "Lala store" -f database/seed_products_always.sql
```

**macOS/Linux:**
```bash
psql -U postgres -d "Lala store" -f database/seed_products_always.sql
```

This inserts 8 sample products only when the `products` table is **empty**.

## Option 3: Full schema + seed (fresh DB)

If the database is new or you want to reset:

1. Run the schema (creates tables and inserts categories/users/products if empty):
   ```powershell
   psql -U postgres -d "Lala store" -f database/schema.sql
   ```
2. If you already ran schema before and products are still empty, run Option 2:
   ```powershell
   psql -U postgres -d "Lala store" -f database/seed_products_always.sql
   ```

## Check that it worked

- **API:** Open http://localhost:8001/api/products — you should see a JSON list of products.
- **Health:** http://localhost:8001/api/health — `products_count` should be > 0.

Products only show when **stock_quantity > 0**. The seeds above set stock to 15–50, so they will appear.
