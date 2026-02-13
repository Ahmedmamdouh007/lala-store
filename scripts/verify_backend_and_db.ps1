# Verify Backend and Database for LALA STORE
# Run from project root: .\scripts\verify_backend_and_db.ps1

$ErrorActionPreference = "Stop"
$projectRoot = Split-Path -Parent (Split-Path -Parent $PSCommandPath)
Set-Location $projectRoot

Write-Host "`n=== LALA STORE - Backend & Database Verification ===" -ForegroundColor Cyan
Write-Host ""

# 1. Check PostgreSQL is listening
Write-Host "[1/4] Checking PostgreSQL (port 5432)..." -ForegroundColor Yellow
$listening = netstat -ano 2>$null | Select-String "LISTENING" | Select-String ":5432\s"
if ($listening) {
    Write-Host "  OK - PostgreSQL is listening on port 5432" -ForegroundColor Green
} else {
    Write-Host "  FAIL - Nothing listening on 5432. Start PostgreSQL service." -ForegroundColor Red
    Write-Host "  Example: Start-Service postgresql-x64-* (or use Services.msc)" -ForegroundColor Gray
    exit 1
}

# 2. Test database connection (Node script or psql)
Write-Host "[2/4] Testing database connection..." -ForegroundColor Yellow
$nodeEnsure = Join-Path $projectRoot "scripts\ensure_database.js"
if (Test-Path $nodeEnsure) {
    $out = & node $nodeEnsure 2>&1
    if ($LASTEXITCODE -eq 0) {
        Write-Host "  OK - Database 'Lala store' connected and schema applied" -ForegroundColor Green
        $out | Where-Object { $_ -match "Tables:|Products:" } | ForEach-Object { Write-Host "    $_" -ForegroundColor Gray }
    } else {
        Write-Host "  FAIL - $out" -ForegroundColor Red
    }
} else {
    $psqlPath = $null
    if (Get-Command psql -ErrorAction SilentlyContinue) { $psqlPath = (Get-Command psql).Source }
    if (-not $psqlPath) {
        $paths = @("C:\Program Files\PostgreSQL\17\bin\psql.exe","C:\Program Files\PostgreSQL\16\bin\psql.exe","C:\Program Files\PostgreSQL\15\bin\psql.exe")
        foreach ($p in $paths) { if (Test-Path $p) { $psqlPath = $p; break } }
    }
    if ($psqlPath) {
        $env:PGPASSWORD = "1234"
        $r = & $psqlPath -U postgres -d "Lala store" -t -c "SELECT 1;" 2>&1
        if ($LASTEXITCODE -eq 0) {
            Write-Host "  OK - Connected to database 'Lala store'" -ForegroundColor Green
        } else {
            Write-Host "  FAIL - Could not connect. Run: node scripts\ensure_database.js" -ForegroundColor Red
        }
        $env:PGPASSWORD = ""
    } else {
        Write-Host "  Run: node scripts\ensure_database.js (requires npm install at project root)" -ForegroundColor Yellow
    }
}

# 3. Backend config
Write-Host "[3/4] Backend config..." -ForegroundColor Yellow
$cfg = Join-Path $projectRoot "backend\config\db_config.json"
if (Test-Path $cfg) {
    Write-Host "  OK - db_config.json exists" -ForegroundColor Green
} else {
    Write-Host "  FAIL - backend\config\db_config.json not found" -ForegroundColor Red
}

# 4. Backend executable
Write-Host "[4/4] Backend executable..." -ForegroundColor Yellow
$exe = Join-Path $projectRoot "backend\build\Release\lala_store.exe"
if (Test-Path $exe) {
    Write-Host "  OK - lala_store.exe found. Start with: .\backend\build\Release\lala_store.exe" -ForegroundColor Green
} else {
    Write-Host "  WARN - Backend not built. From project root:" -ForegroundColor Yellow
    Write-Host "    cd backend\build" -ForegroundColor Gray
    Write-Host "    cmake .. -G `"Visual Studio 17 2022`" -A x64" -ForegroundColor Gray
    Write-Host "    cmake --build . --config Release" -ForegroundColor Gray
}

Write-Host "`n=== Done ===" -ForegroundColor Cyan
Write-Host ""
