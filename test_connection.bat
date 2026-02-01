@echo off
REM LALA STORE - Connection Test Script (Windows)
REM Tests database, backend, and frontend connections

echo ==========================================
echo   LALA STORE - Connection Test
echo ==========================================
echo.

REM Test 1: PostgreSQL Connection
echo Test 1: PostgreSQL Connection
set PGPASSWORD=1234
psql -U postgres -d "Lala store" -c "SELECT 1;" >nul 2>&1
if %errorLevel% equ 0 (
    echo [OK] PostgreSQL connection successful
    
    for /f "tokens=*" %%i in ('psql -U postgres -d "Lala store" -t -c "SELECT COUNT(*) FROM information_schema.tables WHERE table_schema = 'public';" 2^>nul') do set TABLE_COUNT=%%i
    echo [INFO] Found !TABLE_COUNT! tables in database
    
    for /f "tokens=*" %%i in ('psql -U postgres -d "Lala store" -t -c "SELECT COUNT(*) FROM products;" 2^>nul') do set PRODUCT_COUNT=%%i
    echo [INFO] Found !PRODUCT_COUNT! products
) else (
    echo [ERROR] PostgreSQL connection failed
    echo [INFO] Check if PostgreSQL is running
    exit /b 1
)
set PGPASSWORD=

REM Test 2: Backend API
echo.
echo Test 2: Backend API
curl -s http://localhost:8001/api/home/featured >nul 2>&1
if %errorLevel% equ 0 (
    echo [OK] Backend API is responding
    
    curl -s http://localhost:8001/api/home/featured | findstr "success" >nul
    if %errorLevel% equ 0 (
        echo [OK] Backend API endpoint working correctly
    ) else (
        echo [ERROR] Backend API returned unexpected response
    )
) else (
    echo [ERROR] Backend API is not responding
    echo [INFO] Make sure backend is running: cd backend\build\Release ^&^& lala_store.exe
)

REM Test 3: Frontend
echo.
echo Test 3: Frontend
curl -s http://localhost:3001 >nul 2>&1
if %errorLevel% equ 0 (
    echo [OK] Frontend is responding
) else (
    echo [ERROR] Frontend is not responding
    echo [INFO] Make sure frontend is running: cd frontend ^&^& npm run dev
)

REM Test 4: Database Configuration
echo.
echo Test 4: Database Configuration
if exist "backend\config\db_config.json" (
    findstr /C:"Lala store" backend\config\db_config.json >nul
    if %errorLevel% equ 0 (
        findstr /C:"postgres" backend\config\db_config.json >nul
        if %errorLevel% equ 0 (
            echo [OK] Database configuration is correct
        ) else (
            echo [ERROR] Database user mismatch in configuration
        )
    ) else (
        echo [ERROR] Database name mismatch in configuration
    )
) else (
    echo [ERROR] Database configuration file not found
)

echo.
echo ==========================================
echo   Connection Test Complete
echo ==========================================
echo.

pause
