# Quick Setup Guide for LALA STORE

## Prerequisites Check

Before starting, ensure you have:
- ✅ PostgreSQL installed and running
- ✅ C++ compiler (GCC/Clang) with C++17
- ✅ CMake 3.15+
- ✅ Node.js and npm

## Step-by-Step Setup

### 1. Database Setup (5 minutes)

```bash
# Connect to PostgreSQL
psql -U postgres

# Create database
CREATE DATABASE "Lala store";

# Exit psql
\q

# Run schema
psql -U postgres -d "Lala store" -f database/schema.sql
```

**Verify:** Check if tables were created:
```bash
psql -U postgres -d "Lala store" -c "\dt"
```

### 2. Backend Setup (10 minutes)

#### Option A: Using Crow as Git Submodule (Recommended)

```bash
cd backend
git clone https://github.com/CrowCpp/Crow.git crow
```

#### Option B: Download Crow Manually

1. Download from: https://github.com/CrowCpp/Crow/releases
2. Extract to `backend/crow/`
3. Ensure structure: `backend/crow/include/crow.h`

#### Install nlohmann/json

```bash
# Option 1: Using vcpkg
vcpkg install nlohmann-json

# Option 2: Download single header
mkdir -p backend/include/nlohmann
cd backend/include/nlohmann
wget https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp
```

#### Build Backend

```bash
cd backend
mkdir build
cd build
cmake ..
make
```

**If build fails:**
- Check PostgreSQL is installed: `pg_config --version`
- Verify Crow headers are in the right place
- Check CMake output for missing dependencies

#### Run Backend

```bash
cd backend/build
./lala_store
```

You should see:
```
Connected to PostgreSQL database successfully!
Starting LALA STORE server on http://localhost:8001
```

**Test:** Open browser to `http://localhost:8001` - should see "LALA STORE API Server is running!"

### 3. Frontend Setup (5 minutes)

```bash
cd frontend
npm install
npm run dev
```

Frontend will start on `http://localhost:3001`

**Test:** Open browser to `http://localhost:3000` - should see the LALA STORE homepage

## Troubleshooting

### Database Connection Issues

**Error:** "Connection to database failed"

**Solutions:**
1. Check PostgreSQL is running:
   ```bash
   pg_isready -U postgres
   ```

2. Verify database exists:
   ```bash
   psql -U postgres -l | grep "Lala store"
   ```

3. Test connection manually:
   ```bash
   psql -U postgres -d "Lala store"
   ```

4. Check credentials in `backend/config/db_config.json`

### Backend Build Issues

**Error:** "Cannot find crow.h"

**Solution:**
- Ensure Crow is in `backend/crow/include/crow.h`
- Or adjust include path in CMakeLists.txt

**Error:** "Cannot find nlohmann/json.hpp"

**Solution:**
- Download json.hpp to `backend/include/nlohmann/json.hpp`
- Or install via package manager

**Error:** "PostgreSQL not found"

**Solution:**
```bash
# Ubuntu/Debian
sudo apt-get install libpq-dev

# macOS
brew install postgresql

# Verify
pg_config --version
```

### Frontend Issues

**Error:** "Cannot connect to API"

**Solutions:**
1. Ensure backend is running on port 8001
2. Check browser console for CORS errors
3. Verify `vite.config.js` proxy settings

**Error:** "Module not found"

**Solution:**
```bash
cd frontend
rm -rf node_modules package-lock.json
npm install
```

## Testing the Application

1. **Test API directly:**
   ```bash
   curl http://localhost:8001/api/home/featured
   ```

2. **Test Frontend:**
   - Open `http://localhost:3000`
   - Navigate to Men/Women pages
   - Add products to cart
   - Go to checkout

3. **Test Database:**
   ```bash
   psql -U postgres -d "Lala store" -c "SELECT * FROM products LIMIT 5;"
   ```

## Default Test Data

The schema includes sample data:
- 1 test user (ID: 1)
- 8 sample products
- 5 categories

Use `user_id = 1` for testing cart and orders.

## Next Steps

- Replace placeholder images with actual product images
- Implement user authentication
- Add admin dashboard
- Configure production settings

## Need Help?

Check the main README.md for detailed documentation.
