@echo off
REM LALA STORE - Start Backend Script (Windows)

echo Starting LALA STORE Backend...
echo.

cd /d "%~dp0backend"

REM Check if build directory exists
if not exist "build" (
    echo [ERROR] Build directory not found. Please run setup script first.
    pause
    exit /b 1
)

cd build

REM Check for executable in Release folder (Visual Studio) or root (MinGW)
if exist "Release\lala_store.exe" (
    set EXECUTABLE=Release\lala_store.exe
) else if exist "lala_store.exe" (
    set EXECUTABLE=lala_store.exe
) else (
    echo [ERROR] Backend executable not found. Please build the project first.
    pause
    exit /b 1
)

REM Test database connection before starting
set PGPASSWORD=1234
psql -U postgres -d "Lala store" -c "SELECT 1;" >nul 2>&1
if %errorLevel% neq 0 (
    echo [ERROR] Cannot connect to database. Please check PostgreSQL is running.
    echo    Database: Lala store
    echo    User: postgres
    echo    Password: 1234
    set PGPASSWORD=
    pause
    exit /b 1
)
set PGPASSWORD=

echo [OK] Database connection verified
echo [INFO] Starting backend server on http://localhost:8001
echo.

%EXECUTABLE%

pause
