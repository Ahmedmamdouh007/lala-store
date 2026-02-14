@echo off
REM LALA STORE - Start Backend (Windows)
REM Tries MinGW build first, then Visual Studio build.
REM Uses SQLite: database/lala-store.db

echo Starting LALA STORE Backend...
echo.

cd /d "%~dp0"

REM 1) Prefer MinGW build (no Visual Studio required)
if exist "backend\build_mingw\lala_store.exe" (
    REM Add MSYS2 MinGW bin to PATH so DLLs (e.g. libgcc, libstdc++, libsqlite3) are found
    if defined MSYS2_MINGW64 set "MINGW_BIN=%MSYS2_MINGW64%\bin"
    if not defined MINGW_BIN set "MINGW_BIN=C:\msys64\mingw64\bin"
    if exist "%MINGW_BIN%\libgcc_s_seh-1.dll" set "PATH=%MINGW_BIN%;%PATH%"
    echo [INFO] Using MinGW build. Database: database\lala-store.db
    echo [INFO] Starting backend on http://localhost:8005
    echo.
    backend\build_mingw\lala_store.exe
    pause
    exit /b 0
)

REM 2) Visual Studio build
if not exist "backend\build" (
    echo [ERROR] No build found. Build first:
    echo   MinGW:  cd backend ^&^& powershell -ExecutionPolicy Bypass -File build_mingw.ps1
    echo   or VS:  cd backend ^&^& cmake -B build -G "Visual Studio 17 2022" -A x64 ^&^& cmake --build build --config Release
    pause
    exit /b 1
)

cd backend\build
if exist "Release\lala_store.exe" (
    set EXECUTABLE=Release\lala_store.exe
) else if exist "lala_store.exe" (
    set EXECUTABLE=lala_store.exe
) else (
    echo [ERROR] Backend executable not found in backend\build.
    pause
    exit /b 1
)

echo [INFO] Database: ..\..\database\lala-store.db
echo [INFO] Starting backend on http://localhost:8005
echo.

%EXECUTABLE%
pause
