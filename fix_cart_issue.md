# Fix Cart Error - Cannot Add Items to Cart

## Problem
Users cannot add items to the cart. The backend returns a 500 error or the frontend shows an error message.

## Root Causes

### 1. Missing User in Database (Most Common)
The cart requires a user with `id=1` to exist in the `users` table. If this user doesn't exist, the foreign key constraint will fail.

**Solution:**
```sql
-- Connect to PostgreSQL
psql -U postgres -d "Lala store"

-- Ensure user with id=1 exists
INSERT INTO users (id, username, email, password_hash) 
VALUES (1, 'testuser', 'test@example.com', '$2b$10$example_hash_replace_in_production')
ON CONFLICT (id) DO NOTHING
ON CONFLICT (username) DO UPDATE SET id = 1 WHERE users.username = 'testuser';
```

Or run the provided script:
```bash
psql -U postgres -d "Lala store" -f ensure_database_setup.sql
```

### 2. Backend Not Running
The backend server must be running on port 8001.

**Check:**
```bash
curl http://localhost:8001/api/health
```

**Start backend:**
```bash
cd backend/build/Release
./lala_store.exe
```

### 3. Database Connection Issues
The backend cannot connect to PostgreSQL.

**Check:**
- PostgreSQL is running: `netstat -ano | findstr :5432`
- Database exists: `psql -U postgres -l | grep "Lala store"`
- Config is correct: Check `backend/config/db_config.json`

### 4. Missing Products
If products don't exist, adding to cart will fail.

**Solution:**
```sql
-- Run the schema to create products
psql -U postgres -d "Lala store" -f database/schema.sql
```

## Quick Fix Steps

1. **Ensure user exists:**
   ```sql
   psql -U postgres -d "Lala store" -c "INSERT INTO users (id, username, email, password_hash) VALUES (1, 'testuser', 'test@example.com', 'hash') ON CONFLICT (id) DO NOTHING;"
   ```

2. **Verify backend is running:**
   ```bash
   curl http://localhost:8001/api/health
   ```

3. **Check browser console** for detailed error messages

4. **Rebuild backend** if you made code changes:
   ```bash
   cd backend/build
   cmake --build . --config Release
   ```

## Testing

After fixing, test the cart:
1. Open browser: http://localhost:3001
2. Open browser console (F12)
3. Try adding a product to cart
4. Check console for any errors
5. Check network tab to see the API request/response

## Error Messages

- **"User not found"** → User with id=1 doesn't exist
- **"Product not found"** → Product doesn't exist in database
- **"Database connection failed"** → Backend can't connect to PostgreSQL
- **"Backend server is not responding"** → Backend not running on port 8001
- **"Failed to add to cart: violates foreign key constraint"** → User or product doesn't exist

## Code Changes Made

1. Added user validation in `backend/routes/cart_routes.cpp` - checks if user exists before adding to cart
2. Improved error messages in backend - better error reporting for foreign key violations
3. Improved frontend error handling - shows specific error messages from backend
