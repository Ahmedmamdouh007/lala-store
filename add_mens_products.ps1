# Ensure men's products are in the database and run API test
# Database: Lala store | User: postgres | Password: 1234

$ErrorActionPreference = "Stop"
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $scriptDir

Write-Host "LALA STORE - Ensure men's products in database and test" -ForegroundColor Cyan
Write-Host ""

# Try to find PostgreSQL (common paths + PATH)
$pgPaths = @(
    "C:\Program Files\PostgreSQL\17\bin\psql.exe",
    "C:\Program Files\PostgreSQL\16\bin\psql.exe",
    "C:\Program Files\PostgreSQL\15\bin\psql.exe",
    "C:\Program Files\PostgreSQL\14\bin\psql.exe",
    "C:\Program Files (x86)\PostgreSQL\16\bin\psql.exe",
    "C:\Program Files (x86)\PostgreSQL\15\bin\psql.exe",
    "C:\Program Files (x86)\PostgreSQL\14\bin\psql.exe"
)
$psqlPath = $null
foreach ($path in $pgPaths) {
    if (Test-Path $path) { $psqlPath = $path; break }
}
if (-not $psqlPath) {
    $inPath = Get-Command psql -ErrorAction SilentlyContinue
    if ($inPath) { $psqlPath = $inPath.Source }
}
if (-not $psqlPath) {
    Write-Host "PostgreSQL (psql) not found. Add PostgreSQL bin to PATH or install PostgreSQL." -ForegroundColor Red
    exit 1
}
Write-Host "Using: $psqlPath" -ForegroundColor Green

$sqlFile = Join-Path $scriptDir "database\ensure_men_products.sql"
if (-not (Test-Path $sqlFile)) {
    Write-Host "SQL file not found: $sqlFile" -ForegroundColor Red
    exit 1
}

$env:PGPASSWORD = "1234"
Write-Host "Running database script: ensure_men_products.sql" -ForegroundColor Cyan
$result = & $psqlPath -U postgres -d "Lala store" -f $sqlFile 2>&1
Remove-Item Env:\PGPASSWORD -ErrorAction SilentlyContinue

if ($LASTEXITCODE -ne 0) {
    Write-Host "Database script failed:" -ForegroundColor Red
    Write-Host $result -ForegroundColor Red
    exit 1
}
Write-Host "Database script completed successfully." -ForegroundColor Green

# Verify count
$env:PGPASSWORD = "1234"
$countResult = & $psqlPath -U postgres -d "Lala store" -t -c "SELECT COUNT(*) FROM products WHERE gender = 'men';" 2>&1
Remove-Item Env:\PGPASSWORD -ErrorAction SilentlyContinue
$count = ($countResult | Out-String).Trim()
Write-Host "Men's products in database: $count" -ForegroundColor Green
Write-Host ""

# Run API test (backend must be running on port 8001)
Write-Host "Running API test (backend must be on http://localhost:8001)..." -ForegroundColor Cyan
$testScript = Join-Path $scriptDir "scripts\test_men_products_api.js"
if (Test-Path $testScript) {
    node $testScript 2>&1
    if ($LASTEXITCODE -eq 0) {
        Write-Host ""
        Write-Host "All checks passed. Men's page and database are working." -ForegroundColor Green
    } else {
        Write-Host ""
        Write-Host "API test failed. Start the backend (port 8001) and run: node scripts\test_men_products_api.js" -ForegroundColor Yellow
    }
} else {
    Write-Host "Test script not found: $testScript" -ForegroundColor Yellow
}
