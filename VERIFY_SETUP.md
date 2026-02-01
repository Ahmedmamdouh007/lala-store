# Setup Verification Guide

## ✅ Post-Setup Verification Checklist

After running the setup script, verify everything is working:

### 1. Database Verification

```bash
# Test PostgreSQL connection
psql -U postgres -d "Lala store" -c "SELECT COUNT(*) FROM products;"
```

**Expected Output:** A number (should be 8 if sample data loaded)

**If it fails:**
- Check PostgreSQL is running: `pg_isready -U postgres`
- Verify password: `psql -U postgres -c "ALTER USER postgres WITH PASSWORD '1234';"`
- Check database exists: `psql -U postgres -l | grep "Lala store"`

### 2. Backend Verification

```bash
# macOS/Linux
cd backend/build
./lala_store

# Windows
cd backend\build\Release
lala_store.exe
```

**Expected Output:**
```
Connected to PostgreSQL database successfully!
Starting LALA STORE server on http://localhost:8001
API endpoints available at http://localhost:8001/api/
```

**If it fails:**
- Check build logs: `backend/build/cmake_output.log` and `backend/build/build_output.log`
- Verify Crow framework: `ls backend/crow/include/crow.h`
- Verify json library: `ls backend/include/nlohmann/json.hpp`
- Check database config: `cat backend/config/db_config.json`

### 3. Backend API Test

In another terminal:
```bash
curl http://localhost:8001/api/home/featured
```

**Expected Output:** JSON with products array

**If it fails:**
- Backend not running? Start it first
- Port 8001 in use? Check: `lsof -i :8001` (macOS) or `netstat -ano | findstr :8001` (Windows)

### 4. Frontend Verification

```bash
cd frontend
npm run dev
```

**Expected Output:**
```
  VITE v5.x.x  ready in xxx ms

  ➜  Local:   http://localhost:3001/
  ➜  Network: use --host to expose
```

**If it fails:**
- Reinstall dependencies: `rm -rf node_modules && npm install`
- Check Node.js version: `node --version` (should be 16+)

### 5. Frontend-Backend Connection Test

Open browser: `http://localhost:3001`

**Expected:**
- Homepage loads
- Products display
- No console errors (F12 → Console)

**If products don't load:**
- Check browser console for errors
- Verify backend is running on port 8001
- Check network tab for API calls

### 6. Database Connection from Backend

When backend starts, look for:
```
Connected to PostgreSQL database successfully!
```

**If you see:**
```
Connection to database failed: ...
```

**Troubleshoot:**
1. Verify config file path (backend/config/db_config.json)
2. Check database name has space: "Lala store" (not "Lala_store")
3. Test connection manually:
   ```bash
   psql -U postgres -d "Lala store"
   ```

## 🔍 Common Issues & Solutions

### Issue: "Database connection failed"

**Causes:**
- PostgreSQL not running
- Wrong password
- Database doesn't exist
- Config file path wrong

**Solutions:**
```bash
# 1. Start PostgreSQL
# macOS:
brew services start postgresql@15

# Windows:
net start postgresql-x64-15

# 2. Set password
psql -U postgres -c "ALTER USER postgres WITH PASSWORD '1234';"

# 3. Create database
psql -U postgres -c 'CREATE DATABASE "Lala store";'

# 4. Verify config
cat backend/config/db_config.json
```

### Issue: "Cannot find crow.h"

**Solution:**
```bash
cd backend
git clone https://github.com/CrowCpp/Crow.git crow
```

### Issue: "Cannot find nlohmann/json.hpp"

**Solution:**
```bash
mkdir -p backend/include/nlohmann
curl -L -o backend/include/nlohmann/json.hpp \
  https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp
```

### Issue: Backend builds but crashes on start

**Check:**
1. Database connection (see above)
2. Config file exists: `ls backend/config/db_config.json`
3. Config file readable from build directory
4. Database name has space: "Lala store"

**Fix config path if needed:**
The backend looks for config in multiple locations. If it fails, copy config to build directory:
```bash
mkdir -p backend/build/config
cp backend/config/db_config.json backend/build/config/
```

### Issue: Frontend can't connect to backend

**Check:**
1. Backend running? `curl http://localhost:8001`
2. CORS enabled? Check backend/main.cpp
3. Proxy configured? Check frontend/vite.config.js

**Test API directly:**
```bash
curl http://localhost:8001/api/home/featured
```

## 📋 Quick Test Commands

Run these to verify everything:

```bash
# 1. Database
psql -U postgres -d "Lala store" -c "SELECT COUNT(*) FROM products;"

# 2. Backend API (with backend running)
curl http://localhost:8001/api/home/featured | jq .

# 3. Frontend (with frontend running)
curl http://localhost:3001

# 4. Full test script
./test_connection.sh  # macOS/Linux
test_connection.bat   # Windows
```

## ✅ Success Criteria

You're all set when:

- ✅ Database connection successful
- ✅ Backend starts without errors
- ✅ Backend API responds with JSON
- ✅ Frontend starts without errors
- ✅ Browser shows homepage with products
- ✅ Can add products to cart
- ✅ No errors in browser console
- ✅ No errors in backend terminal

## 🎯 Next Steps

Once verified:

1. Explore the website at http://localhost:3001
2. Test adding products to cart
3. Test checkout process
4. Check database to see cart/order data

## 📞 Still Having Issues?

1. Check logs:
   - Backend: `backend/build/cmake_output.log`, `backend/build/build_output.log`
   - Frontend: Browser console (F12)
   - Database: PostgreSQL logs

2. Run test scripts:
   - `./test_connection.sh` (macOS/Linux)
   - `test_connection.bat` (Windows)

3. Verify all prerequisites:
   - PostgreSQL installed and running
   - Node.js 16+ installed
   - CMake 3.15+ installed
   - C++ compiler available

4. Check configuration files:
   - `backend/config/db_config.json`
   - `frontend/vite.config.js`
   - `frontend/package.json`
