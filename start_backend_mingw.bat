@echo off
REM LALA STORE - Start Backend (MinGW build, no Visual Studio)
REM Run from project root. Uses database/lala-store.db

cd /d "%~dp0"

if not exist "backend\build_mingw\lala_store.exe" (
    echo [ERROR] MinGW build not found. Run: cd backend ^&^& powershell -ExecutionPolicy Bypass -File build_mingw.ps1
    pause
    exit /b 1
)

REM Add MSYS2 MinGW bin to PATH so DLLs (libgcc, libstdc++, libsqlite3, etc.) are found
if defined MSYS2_MINGW64 set "MINGW_BIN=%MSYS2_MINGW64%\bin"
if not defined MINGW_BIN set "MINGW_BIN=C:\msys64\mingw64\bin"
if exist "%MINGW_BIN%\libgcc_s_seh-1.dll" set "PATH=%MINGW_BIN%;%PATH%"

echo [INFO] Starting backend (MinGW) on http://localhost:8005
echo [INFO] Database: database\lala-store.db
echo.

backend\build_mingw\lala_store.exe

pause
