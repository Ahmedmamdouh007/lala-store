# LALA STORE - Setup Verification Script
# Checks database, backend build status, and provides next steps

Write-Host "`n========================================" -ForegroundColor Cyan
Write-Host "LALA STORE - Setup Verification" -ForegroundColor Cyan
Write-Host "========================================`n" -ForegroundColor Cyan

$projectRoot = $PSScriptRoot
if (-not $projectRoot) {
    $projectRoot = Get-Location
}

# 1. Check Database
Write-Host "[1] Checking Database..." -ForegroundColor Yellow
$dbPath = Join-Path $projectRoot "database\lala-store.db"
if (Test-Path $dbPath) {
    $dbSize = (Get-Item $dbPath).Length / 1KB
    Write-Host "  ✓ Database exists: $dbPath ($([math]::Round($dbSize, 2)) KB)" -ForegroundColor Green
    
    # Try to check product count using Node.js
    $nodeCheck = node -e "
        const fs = require('fs');
        const path = require('path');
        const initSqlJs = require('sql.js');
        (async () => {
            const SQL = await initSqlJs();
            const dbPath = '$dbPath'.replace(/\\/g, '/');
            if (fs.existsSync(dbPath)) {
                const fileBuffer = fs.readFileSync(dbPath);
                const db = new SQL.Database(fileBuffer);
                const result = db.exec('SELECT COUNT(*) AS c FROM products');
                const count = result.length && result[0].values[0][0] || 0;
                console.log(count);
                db.close();
            } else {
                console.log('0');
            }
        })();
    " 2>$null
    
    if ($nodeCheck -and [int]$nodeCheck -gt 0) {
        Write-Host "  ✓ Products in database: $nodeCheck" -ForegroundColor Green
    } else {
        Write-Host "  ⚠ No products found. Run: npm run ensure-db" -ForegroundColor Yellow
    }
} else {
    Write-Host "  ✗ Database not found. Run: npm run ensure-db" -ForegroundColor Red
}

# 2. Check Backend Build
Write-Host "`n[2] Checking Backend Build..." -ForegroundColor Yellow
$backendExe1 = Join-Path $projectRoot "backend\build\Release\lala_store.exe"
$backendExe2 = Join-Path $projectRoot "backend\build\lala_store.exe"

if (Test-Path $backendExe1) {
    Write-Host "  ✓ Backend executable found: $backendExe1" -ForegroundColor Green
    $backendExe = $backendExe1
} elseif (Test-Path $backendExe2) {
    Write-Host "  ✓ Backend executable found: $backendExe2" -ForegroundColor Green
    $backendExe = $backendExe2
} else {
    Write-Host "  ✗ Backend not built. Build required:" -ForegroundColor Red
    Write-Host "    cd backend" -ForegroundColor Gray
    Write-Host "    cmake -B build -G `"Visual Studio 17 2022`"" -ForegroundColor Gray
    Write-Host "    cmake --build build --config Release" -ForegroundColor Gray
    $backendExe = $null
}

# 3. Check Backend Running
Write-Host "`n[3] Checking Backend Server..." -ForegroundColor Yellow
try {
    $response = Invoke-WebRequest -Uri "http://localhost:8001/api/health" -TimeoutSec 2 -ErrorAction Stop
    $health = $response.Content | ConvertFrom-Json
    if ($health.ok -and $health.database) {
        Write-Host "  ✓ Backend is running and connected to database" -ForegroundColor Green
        Write-Host "    Products: $($health.products_count)" -ForegroundColor Gray
    } else {
        Write-Host "  ⚠ Backend running but database not connected" -ForegroundColor Yellow
    }
} catch {
    Write-Host "  ✗ Backend not running on port 8001" -ForegroundColor Red
    if ($backendExe) {
        Write-Host "    Start with: start_backend.bat" -ForegroundColor Gray
    }
}

# 4. Check Frontend
Write-Host "`n[4] Checking Frontend Configuration..." -ForegroundColor Yellow
$viteConfig = Join-Path $projectRoot "frontend\vite.config.js"
if (Test-Path $viteConfig) {
    $configContent = Get-Content $viteConfig -Raw
    if ($configContent -match "127\.0\.0\.1:8001" -or $configContent -match "localhost:8001") {
        Write-Host "  ✓ Frontend proxy configured correctly" -ForegroundColor Green
    } else {
        Write-Host "  ⚠ Frontend proxy may not be configured" -ForegroundColor Yellow
    }
} else {
    Write-Host "  ⚠ vite.config.js not found" -ForegroundColor Yellow
}

# Summary
Write-Host "`n========================================" -ForegroundColor Cyan
Write-Host "Summary" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan

Write-Host "`nNext Steps:" -ForegroundColor Yellow

if (-not (Test-Path $dbPath)) {
    Write-Host "1. Setup database: npm run ensure-db" -ForegroundColor White
}

if (-not $backendExe) {
    Write-Host "2. Build backend (see BACKEND_SETUP_GUIDE.md)" -ForegroundColor White
}

if ($backendExe -and (Test-Path $backendExe)) {
    Write-Host "3. Start backend: start_backend.bat" -ForegroundColor White
    Write-Host "4. Start frontend: cd frontend ; npm run dev" -ForegroundColor White
    Write-Host "5. Open browser: http://localhost:3001" -ForegroundColor White
} else {
    Write-Host "3. Build backend first (see step 2)" -ForegroundColor White
}

Write-Host "`nFor detailed instructions, see: BACKEND_SETUP_GUIDE.md`n" -ForegroundColor Gray
