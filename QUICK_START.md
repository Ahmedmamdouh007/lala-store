# Quick Start Guide - LALA STORE

## 🚀 Quick Setup (Choose Your OS)

### macOS Setup
```bash
./setup_macos.sh
```

### Windows Setup
```cmd
setup_windows.bat
```

## 📋 What the Setup Scripts Do

1. ✅ **Install Dependencies**
   - PostgreSQL (database)
   - Node.js & npm (frontend)
   - CMake & C++ tools (backend)
   - Git (for downloading frameworks)

2. ✅ **Setup Database**
   - Create database "Lala store"
   - Set user: postgres, password: 1234
   - Load schema and sample data

3. ✅ **Download Frameworks**
   - Crow C++ framework
   - nlohmann/json library

4. ✅ **Build Backend**
   - Configure with CMake
   - Compile C++ code
   - Create executable

5. ✅ **Setup Frontend**
   - Install npm dependencies
   - Configure React app

6. ✅ **Verify Connections**
   - Test database connection
   - Verify configuration files

## 🧪 Test Connections

After setup, test everything:

### macOS/Linux:
```bash
./test_connection.sh
```

### Windows:
```cmd
test_connection.bat
```

## 🎯 Start the Application

### Terminal 1 - Backend:
```bash
# macOS/Linux
cd backend/build
./lala_store

# Windows
cd backend\build\Release
lala_store.exe
```

### Terminal 2 - Frontend:
```bash
cd frontend
npm run dev
```

### Open Browser:
```
http://localhost:3001
```

## 🔧 Manual Database Setup (If Script Fails)

```bash
# Connect to PostgreSQL
psql -U postgres

# Create database
CREATE DATABASE "Lala store";

# Set password (if needed)
ALTER USER postgres WITH PASSWORD '1234';

# Exit and run schema
\q
psql -U postgres -d "Lala store" -f database/schema.sql
```

## 🐛 Troubleshooting

### Database Connection Issues

**Error:** "Connection to database failed"

**Solutions:**
1. Check PostgreSQL is running:
   ```bash
   # macOS
   pg_isready -U postgres
   
   # Windows
   sc query postgresql-x64-15
   ```

2. Verify database exists:
   ```bash
   psql -U postgres -l | grep "Lala store"
   ```

3. Test connection manually:
   ```bash
   psql -U postgres -d "Lala store"
   ```

4. Check password:
   ```sql
   psql -U postgres -c "ALTER USER postgres WITH PASSWORD '1234';"
   ```

### Backend Build Issues

**Error:** "Cannot find crow.h"

**Solution:**
```bash
cd backend
git clone https://github.com/CrowCpp/Crow.git crow
```

**Error:** "Cannot find nlohmann/json.hpp"

**Solution:**
```bash
mkdir -p backend/include/nlohmann
# Download from: https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp
# Or use curl:
curl -L -o backend/include/nlohmann/json.hpp https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp
```

**Error:** "PostgreSQL not found"

**Solution:**
- macOS: `brew install postgresql@15`
- Windows: Install from https://www.postgresql.org/download/windows/

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

## 📊 Database Configuration

The database is configured in `backend/config/db_config.json`:

```json
{
  "host": "localhost",
  "port": 5432,
  "database": "Lala store",
  "user": "postgres",
  "password": "1234"
}
```

**Important:** The database name has a space: "Lala store" (not "Lala_store")

## ✅ Verification Checklist

After setup, verify:

- [ ] PostgreSQL is running
- [ ] Database "Lala store" exists
- [ ] Backend builds without errors
- [ ] Frontend dependencies installed
- [ ] Backend starts on port 8001
- [ ] Frontend starts on port 3001
- [ ] Can access http://localhost:3001
- [ ] Can see products on homepage
- [ ] Can add products to cart

## 🎉 Success Indicators

You'll know everything is working when:

1. Backend shows: `Connected to PostgreSQL database successfully!`
2. Backend shows: `Starting LALA STORE server on http://localhost:8001`
3. Frontend shows: `Local: http://localhost:3001`
4. Browser shows the LALA STORE homepage with products
5. No errors in browser console

## 📞 Need Help?

1. Run the test connection script
2. Check the logs in `backend/build/` and `frontend/`
3. Verify all prerequisites are installed
4. Check the main README.md for detailed documentation
