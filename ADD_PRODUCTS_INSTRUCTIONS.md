# Instructions to Add Men's Products to Database

## ✅ Completed Tasks

1. **Images Copied**: All 5 product images have been copied to `frontend/public/images/`:
   - `mercedes-amg-f1-tee.png`
   - `remember-who-you-wanted-tee.png`
   - `abstract-black-grey-shirt.png`
   - `tropical-palm-shirt.png`
   - `diadora-abstract-tee.png`

2. **SQL File Created**: `database/add_mens_products.sql` contains all 5 products with:
   - Titles and descriptions
   - Prices
   - Size options (S, M, L, XL, XXL)
   - Size charts with measurements

3. **Backend Updated**: Updated `backend/routes/home_routes.cpp` to include `sizes` and `size_chart` in the featured products API response.

## 📋 Products to Add

1. **Mercedes-AMG Petronas Formula One Team White T-Shirt** - $34.99
2. **Remember Who You Wanted To Be Black T-Shirt** - $28.99
3. **Abstract Wave Print Short Sleeve Button-Down Shirt** - $49.99
4. **Tropical Palm Tree Print Button-Up Shirt** - $44.99
5. **Diadora Abstract Graphic White T-Shirt** - $36.99

## 🚀 How to Add Products to Database

### Option 1: Using PowerShell Script (Recommended)

Run the PowerShell script I created:

```powershell
powershell -ExecutionPolicy Bypass -File add_mens_products.ps1
```

**Note**: If PostgreSQL is not found, you'll need to either:
- Add PostgreSQL to your PATH, OR
- Modify `add_mens_products.ps1` to include your PostgreSQL installation path

### Option 2: Manual SQL Execution

If you have PostgreSQL installed and accessible:

1. **Find your PostgreSQL installation path** (common locations):
   - `C:\Program Files\PostgreSQL\16\bin\psql.exe`
   - `C:\Program Files\PostgreSQL\15\bin\psql.exe`
   - `C:\Program Files\PostgreSQL\14\bin\psql.exe`

2. **Run the SQL file**:
   ```powershell
   # Set password environment variable
   $env:PGPASSWORD = "1234"
   
   # Run SQL file (replace path with your PostgreSQL installation)
   & "C:\Program Files\PostgreSQL\16\bin\psql.exe" -U postgres -d "Lala store" -f database\add_mens_products.sql
   
   # Clean up
   Remove-Item Env:\PGPASSWORD
   ```

### Option 3: Using pgAdmin or Other Database Tool

1. Open pgAdmin or your preferred PostgreSQL client
2. Connect to your database: `Lala store`
3. Open the SQL file: `database/add_mens_products.sql`
4. Execute the SQL script

## ✅ Verification

After running the SQL, verify the products were added:

```sql
-- Check total men's products
SELECT COUNT(*) FROM products WHERE gender = 'men';

-- View the new products
SELECT name, price, stock_quantity, sizes FROM products WHERE gender = 'men' ORDER BY created_at DESC LIMIT 5;
```

## 📱 How Products Will Appear

### Home Page (`/`)
- The home page shows the **8 most recent products** (ordered by `created_at DESC`)
- Since these are new products, they will appear at the top of the featured products section
- The home page now includes `sizes` and `size_chart` data

### Men's Page (`/men`)
- Shows **all products** where `gender = 'men'`
- Includes all 5 new products
- Products are ordered by `created_at DESC` (newest first)

## 🔄 After Adding Products

1. **Restart Backend** (if running):
   - Stop the backend server (Ctrl+C)
   - Restart it to ensure it picks up any changes

2. **Refresh Frontend**:
   - The frontend will automatically fetch new products from the API
   - Refresh your browser to see the new products

3. **Verify**:
   - Visit `http://localhost:3001` (Home page)
   - Visit `http://localhost:3001/men` (Men's page)
   - You should see all 5 new products displayed

## 📝 Notes

- The SQL file includes duplicate prevention - it won't add products that already exist (by name)
- All products are categorized under "T-Shirts"
- All products have `gender = 'men'`
- All products have stock quantities set (28-55 units)
- All products include size options: S, M, L, XL, XXL
