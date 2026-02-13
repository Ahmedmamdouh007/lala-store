# Test PostgreSQL Database Connection
# Database: Lala store
# User: postgres
# Password: 1234

Write-Host "Testing PostgreSQL Connection..." -ForegroundColor Cyan
Write-Host "Database: Lala store" -ForegroundColor Yellow
Write-Host "User: postgres" -ForegroundColor Yellow
Write-Host "Password: 1234" -ForegroundColor Yellow
Write-Host ""

# Try to find PostgreSQL installation (check PATH first, then common locations)
$psqlPath = $null
if (Get-Command psql -ErrorAction SilentlyContinue) {
    $psqlPath = (Get-Command psql).Source
    Write-Host "Found PostgreSQL (from PATH): $psqlPath" -ForegroundColor Green
}
if (-not $psqlPath) {
    $pgPaths = @(
        "C:\Program Files\PostgreSQL\17\bin\psql.exe",
        "C:\Program Files\PostgreSQL\16\bin\psql.exe",
        "C:\Program Files\PostgreSQL\15\bin\psql.exe",
        "C:\Program Files\PostgreSQL\14\bin\psql.exe",
        "C:\Program Files (x86)\PostgreSQL\17\bin\psql.exe",
        "C:\Program Files (x86)\PostgreSQL\16\bin\psql.exe",
        "C:\Program Files (x86)\PostgreSQL\15\bin\psql.exe",
        "C:\Program Files (x86)\PostgreSQL\14\bin\psql.exe"
    )
    foreach ($path in $pgPaths) {
        if (Test-Path $path) {
            $psqlPath = $path
            Write-Host "Found PostgreSQL at: $path" -ForegroundColor Green
            break
        }
    }
}

if (-not $psqlPath) {
    Write-Host "PostgreSQL not found in common locations." -ForegroundColor Red
    Write-Host "Please ensure PostgreSQL is installed and add it to PATH, or provide the full path to psql.exe" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "To install PostgreSQL:" -ForegroundColor Cyan
    Write-Host "1. Download from https://www.postgresql.org/download/windows/" -ForegroundColor White
    Write-Host "2. Install with password: 1234" -ForegroundColor White
    Write-Host "3. Create database: CREATE DATABASE `"Lala store`";" -ForegroundColor White
    exit 1
}

# Set password environment variable
$env:PGPASSWORD = "1234"

# Test connection
Write-Host "Testing connection..." -ForegroundColor Cyan
$result = & $psqlPath -U postgres -d "Lala store" -c "SELECT 1;" 2>&1

if ($LASTEXITCODE -eq 0) {
    Write-Host "✅ Database connection successful!" -ForegroundColor Green
    
    # Check if tables exist
    Write-Host "Checking tables..." -ForegroundColor Cyan
    $tableCount = & $psqlPath -U postgres -d "Lala store" -t -c "SELECT COUNT(*) FROM information_schema.tables WHERE table_schema = 'public';" 2>&1 | ForEach-Object { $_.ToString().Trim() }
    
    if ($tableCount -match '^\d+$') {
        Write-Host "✅ Found $tableCount tables in database" -ForegroundColor Green
        
        # Check products
        $productCount = & $psqlPath -U postgres -d "Lala store" -t -c "SELECT COUNT(*) FROM products;" 2>&1 | ForEach-Object { $_.ToString().Trim() }
        if ($productCount -match '^\d+$') {
            Write-Host "✅ Found $productCount products" -ForegroundColor Green
        } else {
            Write-Host "⚠️  Products table may be empty or not exist" -ForegroundColor Yellow
            Write-Host "Run: $psqlPath -U postgres -d `"Lala store`" -f database\schema.sql" -ForegroundColor Cyan
        }
    } else {
        Write-Host "⚠️  Database exists but may not have tables" -ForegroundColor Yellow
        Write-Host "Run: $psqlPath -U postgres -d `"Lala store`" -f database\schema.sql" -ForegroundColor Cyan
    }
} else {
    Write-Host "❌ Database connection failed!" -ForegroundColor Red
    Write-Host "Error: $result" -ForegroundColor Red
    Write-Host ""
    Write-Host "Troubleshooting steps:" -ForegroundColor Cyan
    Write-Host "1. Ensure PostgreSQL service is running" -ForegroundColor White
    Write-Host "2. Verify password is set to '1234': $psqlPath -U postgres -c `"ALTER USER postgres WITH PASSWORD '1234';`"" -ForegroundColor White
    Write-Host "3. Create database if it doesn't exist: $psqlPath -U postgres -c `"CREATE DATABASE `"Lala store`";`"" -ForegroundColor White
    $env:PGPASSWORD = ""
    exit 1
}

$env:PGPASSWORD = ""
Write-Host ""
Write-Host "✅ Database connection test completed!" -ForegroundColor Green
