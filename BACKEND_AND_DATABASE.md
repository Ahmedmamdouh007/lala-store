# Backend & Database – Verification and Setup

This doc explains how to ensure the **backend** and **database** for LALA STORE are working.

## Quick verification

From the **project root**:

```powershell
# 1. Run the verification script
.\scripts\verify_backend_and_db.ps1

# 2. Ensure database exists and schema is applied (no psql required)
npm run ensure-db
```

- **PostgreSQL** must be running on port **5432** (same as in `backend/config/db_config.json`).
- **Database name:** `Lala store`  
- **User:** `postgres`  
- **Password:** `1234`

## Database

- **Config:** `backend/config/db_config.json`
- **Schema:** `database/schema.sql`
- **Ensure DB + schema (Node, no psql):**  
  `npm run ensure-db`  
  or  
  `node scripts/ensure_database.js`  
  This creates the database if missing and runs the schema.

If you use **psql**, you can instead:

```powershell
$env:PGPASSWORD = "1234"
psql -U postgres -c 'CREATE DATABASE "Lala store";'
psql -U postgres -d "Lala store" -f database\schema.sql
```

## Backend (C++)

- **Port:** **8001**
- **Config:** Backend looks for `db_config.json` in:
  - `../config/` (when run from `backend/build/Release/`)
  - `config/` (when run from `backend/` or `backend/build/`)
  - `backend/config/` (when run from project root)

If no config file is found, it uses: `localhost:5432`, database `Lala store`, user `postgres`, password `1234`.

### Build (Windows, Visual Studio)

```powershell
cd backend\build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

If Visual Studio is not installed, install **Visual Studio Build Tools** with the “Desktop development with C++” workload, or use another generator (e.g. Ninja) if you have it.

### Run

```powershell
# From backend\build\Release (recommended – config path is correct)
.\backend\build\Release\lala_store.exe
```

Then:

- API: http://localhost:8001  
- Health (includes DB check): http://localhost:8001/api/health  

## Health check

When the backend is running:

```powershell
curl http://localhost:8001/api/health
```

Expected: `ok: true`, `database: true`, and a `products_count` value if the DB and schema are set up.

## Summary

| Component    | How to check / fix |
|------------|---------------------|
| PostgreSQL | Port 5432 listening; run `.\scripts\verify_backend_and_db.ps1` |
| Database   | `npm run ensure-db` (or psql + `database/schema.sql`) |
| Backend    | Build with CMake + VS, run `lala_store.exe`, then open http://localhost:8001/api/health |
