@echo off
REM LALA STORE - Windows Setup Script
REM This script sets up the entire project on Windows

setlocal enabledelayedexpansion

echo ==========================================
echo   LALA STORE - Windows Setup Script
echo ==========================================
echo.

REM Check if running as administrator
net session >nul 2>&1
if %errorLevel% neq 0 (
    echo WARNING: Not running as administrator. Some operations may require admin rights.
    echo.
)

REM Step 1: Check Chocolatey
echo Step 1: Checking Chocolatey...
where choco >nul 2>&1
if %errorLevel% neq 0 (
    echo Installing Chocolatey...
    powershell -NoProfile -ExecutionPolicy Bypass -Command "Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))"
    if %errorLevel% neq 0 (
        echo ERROR: Failed to install Chocolatey
        exit /b 1
    )
    echo Chocolatey installed successfully
) else (
    echo Chocolatey is already installed
)

REM Step 2: Install PostgreSQL
echo.
echo Step 2: Checking PostgreSQL...
where psql >nul 2>&1
if %errorLevel% neq 0 (
    echo Installing PostgreSQL...
    choco install postgresql15 -y
    if %errorLevel% neq 0 (
        echo ERROR: Failed to install PostgreSQL
        echo Please install PostgreSQL manually from https://www.postgresql.org/download/windows/
        exit /b 1
    )
    echo PostgreSQL installed. Please restart your terminal and run this script again.
    echo Or manually add PostgreSQL to PATH and start the service.
    pause
    exit /b 0
) else (
    echo PostgreSQL is installed
)

REM Check if PostgreSQL service is running
sc query postgresql-x64-15 >nul 2>&1
if %errorLevel% equ 0 (
    sc query postgresql-x64-15 | find "RUNNING" >nul
    if %errorLevel% neq 0 (
        echo Starting PostgreSQL service...
        net start postgresql-x64-15
        timeout /t 3 >nul
    )
) else (
    sc query postgresql-x64-14 >nul 2>&1
    if %errorLevel% equ 0 (
        sc query postgresql-x64-14 | find "RUNNING" >nul
        if %errorLevel% neq 0 (
            echo Starting PostgreSQL service...
            net start postgresql-x64-14
            timeout /t 3 >nul
        )
    )
)

REM Step 3: Install Node.js
echo.
echo Step 3: Checking Node.js...
where node >nul 2>&1
if %errorLevel% neq 0 (
    echo Installing Node.js...
    choco install nodejs -y
    if %errorLevel% neq 0 (
        echo ERROR: Failed to install Node.js
        exit /b 1
    )
    echo Please restart your terminal after Node.js installation.
    pause
    exit /b 0
) else (
    echo Node.js is installed
    node --version
)

where npm >nul 2>&1
if %errorLevel% neq 0 (
    echo ERROR: npm not found
    exit /b 1
) else (
    echo npm is installed
    npm --version
)

REM Step 4: Install CMake and C++ tools
echo.
echo Step 4: Checking CMake...
where cmake >nul 2>&1
if %errorLevel% neq 0 (
    echo Installing CMake...
    choco install cmake -y
    if %errorLevel% neq 0 (
        echo ERROR: Failed to install CMake
        exit /b 1
    )
) else (
    echo CMake is installed
    cmake --version
)

REM Check for Visual Studio Build Tools or MinGW
where cl >nul 2>&1
if %errorLevel% neq 0 (
    where g++ >nul 2>&1
    if %errorLevel% neq 0 (
        echo Installing Visual Studio Build Tools...
        choco install visualstudio2022buildtools --package-parameters "--add Microsoft.VisualStudio.Workload.VCTools" -y
        echo Please restart your terminal after Visual Studio Build Tools installation.
        pause
        exit /b 0
    )
)

REM Step 5: Setup Database
echo.
echo Step 5: Setting up database...
set PGPASSWORD=1234

REM Test PostgreSQL connection
psql -U postgres -c "SELECT 1;" >nul 2>&1
if %errorLevel% neq 0 (
    echo ERROR: Cannot connect to PostgreSQL
    echo Please ensure PostgreSQL is running and password is set to "1234"
    echo You can set the password with: psql -U postgres -c "ALTER USER postgres WITH PASSWORD '1234';"
    exit /b 1
)

REM Check if database exists
psql -U postgres -lqt 2>nul | findstr /C:"Lala store" >nul
if %errorLevel% equ 0 (
    echo Database "Lala store" already exists.
    set /p RECREATE="Do you want to drop and recreate it? (y/N): "
    if /i "!RECREATE!"=="y" (
        echo Dropping existing database...
        psql -U postgres -c "DROP DATABASE \"Lala store\";" 2>nul
    ) else (
        echo Using existing database...
    )
)

REM Create database if it doesn't exist
psql -U postgres -lqt 2>nul | findstr /C:"Lala store" >nul
if %errorLevel% neq 0 (
    echo Creating database "Lala store"...
    psql -U postgres -c "CREATE DATABASE \"Lala store\";" 2>nul
    if %errorLevel% neq 0 (
        echo ERROR: Failed to create database
        echo Trying to set password for postgres user...
        psql -U postgres -c "ALTER USER postgres WITH PASSWORD '1234';" 2>nul
        psql -U postgres -c "CREATE DATABASE \"Lala store\";" 2>nul
        if !errorLevel! neq 0 (
            echo ERROR: Failed to create database. Please check PostgreSQL is running.
            exit /b 1
        )
    )
    echo Database created successfully
)

REM Run schema
echo Running database schema...
if exist "database\schema.sql" (
    psql -U postgres -d "Lala store" -f database\schema.sql 2>nul
    if %errorLevel% equ 0 (
        echo Database schema loaded successfully
    ) else (
        echo ERROR: Failed to load schema
        exit /b 1
    )
) else (
    echo ERROR: database\schema.sql not found
    exit /b 1
)

set PGPASSWORD=

REM Step 6: Download Crow Framework
echo.
echo Step 6: Downloading Crow framework...
if not exist "backend\crow" (
    echo Cloning Crow framework...
    cd backend
    git clone https://github.com/CrowCpp/Crow.git crow 2>nul
    if %errorLevel% neq 0 (
        echo ERROR: Failed to clone Crow. Please check internet connection and Git installation.
        exit /b 1
    )
    cd ..
    echo Crow framework downloaded
) else (
    echo Crow framework already exists
)

REM Step 7: Download nlohmann/json
echo.
echo Step 7: Downloading nlohmann/json...
if not exist "backend\include\nlohmann\json.hpp" (
    echo Downloading json.hpp...
    if not exist "backend\include\nlohmann" mkdir backend\include\nlohmann
    powershell -Command "Invoke-WebRequest -Uri 'https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp' -OutFile 'backend\include\nlohmann\json.hpp'"
    if %errorLevel% equ 0 (
        echo nlohmann/json downloaded
    ) else (
        echo ERROR: Failed to download json.hpp
        exit /b 1
    )
) else (
    echo nlohmann/json already exists
)

REM Step 8: Build Backend
echo.
echo Step 8: Building backend...
cd backend
if not exist "build" mkdir build
cd build

echo Running CMake...
cmake .. > cmake_output.log 2>&1
if %errorLevel% neq 0 (
    echo ERROR: CMake configuration failed. Check cmake_output.log for details.
    exit /b 1
)

echo Building project...
cmake --build . --config Release > build_output.log 2>&1
if %errorLevel% neq 0 (
    echo ERROR: Build failed. Check build_output.log for details.
    exit /b 1
)

if exist "Release\lala_store.exe" (
    echo Backend built successfully
) else if exist "lala_store.exe" (
    echo Backend built successfully
) else (
    echo ERROR: Backend executable not found
    exit /b 1
)

cd ..\..

REM Step 9: Setup Frontend
echo.
echo Step 9: Setting up frontend...
cd frontend

if not exist "node_modules" (
    echo Installing npm dependencies...
    call npm install > npm_install.log 2>&1
    if %errorLevel% neq 0 (
        echo ERROR: npm install failed. Check npm_install.log for details.
        exit /b 1
    )
    echo Frontend dependencies installed
) else (
    echo Frontend dependencies already installed
)

cd ..

REM Step 10: Test Database Connection
echo.
echo Step 10: Testing database connection...
set PGPASSWORD=1234
psql -U postgres -d "Lala store" -c "SELECT COUNT(*) FROM products;" >nul 2>&1
if %errorLevel% equ 0 (
    for /f "tokens=*" %%i in ('psql -U postgres -d "Lala store" -t -c "SELECT COUNT(*) FROM products;" 2^>nul') do set PRODUCT_COUNT=%%i
    echo Database connection successful (Found !PRODUCT_COUNT! products)
) else (
    echo ERROR: Database connection test failed
    exit /b 1
)
set PGPASSWORD=

REM Step 11: Verify Configuration
echo.
echo Step 11: Verifying configuration...
if exist "backend\config\db_config.json" (
    echo Database configuration file exists
    findstr /C:"Lala store" backend\config\db_config.json >nul
    if %errorLevel% equ 0 (
        echo Database name in config is correct
    )
) else (
    echo ERROR: Database configuration file not found
    exit /b 1
)

REM Final Summary
echo.
echo ==========================================
echo   Setup Complete!
echo ==========================================
echo.
echo All dependencies installed
echo Database configured and connected
echo Backend built successfully
echo Frontend dependencies installed
echo.
echo To start the application:
echo.
echo 1. Start Backend (Command Prompt 1):
echo    cd backend\build\Release
echo    lala_store.exe
echo    (or cd backend\build if Release folder doesn't exist)
echo.
echo 2. Start Frontend (Command Prompt 2):
echo    cd frontend
echo    npm run dev
echo.
echo 3. Open browser:
echo    http://localhost:3000
echo.
echo Database Info:
echo   Name: Lala store
echo   User: postgres
echo   Password: 1234
echo   Host: localhost
echo   Port: 5432
echo.

pause
