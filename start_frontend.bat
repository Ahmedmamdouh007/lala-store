@echo off
REM LALA STORE - Start Frontend Script (Windows)

echo Starting LALA STORE Frontend...
echo.

cd /d "%~dp0frontend"

REM Check if node_modules exists
if not exist "node_modules" (
    echo [INFO] Dependencies not installed. Installing...
    call npm install
    if %errorLevel% neq 0 (
        echo [ERROR] Failed to install dependencies
        pause
        exit /b 1
    )
)

echo [INFO] Starting frontend development server on http://localhost:3001
echo.

call npm run dev

pause
