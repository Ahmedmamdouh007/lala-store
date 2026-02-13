@echo off
REM LALA STORE - Start Backend (MinGW build, no Visual Studio)
REM Run from project root. Uses database/lala-store.db

cd /d "%~dp0"

if not exist "backend\build_mingw\lala_store.exe" (
    echo [ERROR] MinGW build not found. Run: cd backend ^&^& powershell -ExecutionPolicy Bypass -File build_mingw.ps1
    pause
    exit /b 1
)

echo [INFO] Starting backend (MinGW) on http://localhost:8001
echo [INFO] Database: database\lala-store.db
echo.

backend\build_mingw\lala_store.exe

pause
