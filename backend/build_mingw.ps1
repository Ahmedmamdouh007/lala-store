# Build LALA STORE backend with MinGW (no Visual Studio required)
# Requires: MSYS2 MinGW64 (C:\msys64\mingw64\bin\g++.exe) and CMake in PATH

$ErrorActionPreference = "Stop"
$BackendDir = $PSScriptRoot
$MinGwBin = "C:\msys64\mingw64\bin"
$MinGwPrefix = "C:\msys64\mingw64"

if (-not (Test-Path "$MinGwBin\g++.exe")) {
    Write-Host "MinGW not found at $MinGwBin. Install MSYS2 and run: pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-sqlite3 mingw-w64-x86_64-make" -ForegroundColor Red
    exit 1
}

$env:PATH = "$MinGwBin;$env:PATH"
$BuildDir = Join-Path $BackendDir "build_mingw"

Write-Host "Building backend with MinGW (no Visual Studio)..." -ForegroundColor Cyan
Write-Host "  MinGW: $MinGwBin" -ForegroundColor Gray
Write-Host "  Build dir: $BuildDir" -ForegroundColor Gray

if (Test-Path $BuildDir) {
    Remove-Item -Recurse -Force $BuildDir
}
New-Item -ItemType Directory -Path $BuildDir | Out-Null
Set-Location $BuildDir

# Configure with MinGW Makefiles
& cmake $BackendDir `
    -G "MinGW Makefiles" `
    -DCMAKE_C_COMPILER="$MinGwBin\gcc.exe" `
    -DCMAKE_CXX_COMPILER="$MinGwBin\g++.exe" `
    -DCMAKE_MAKE_PROGRAM="$MinGwBin\mingw32-make.exe" `
    -DCMAKE_PREFIX_PATH="$MinGwPrefix"

if ($LASTEXITCODE -ne 0) {
    Write-Host "CMake configure failed." -ForegroundColor Red
    exit $LASTEXITCODE
}

# Build
& "$MinGwBin\mingw32-make.exe" -j
if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed." -ForegroundColor Red
    exit $LASTEXITCODE
}

Write-Host ""
Write-Host "Build succeeded. Executable: $BuildDir\lala_store.exe" -ForegroundColor Green
Write-Host "Run from project root: .\start_backend_mingw.bat" -ForegroundColor Yellow
