# Fix Cart Database Issue
# Ensures user with id=1 exists in the database

Write-Host "Fixing Cart Database Issue..." -ForegroundColor Cyan
Write-Host ""

# Try to find PostgreSQL psql.exe
$pgPaths = @(
    "C:\Program Files\PostgreSQL\16\bin\psql.exe",
    "C:\Program Files\PostgreSQL\15\bin\psql.exe",
    "C:\Program Files\PostgreSQL\14\bin\psql.exe",
    "C:\Program Files (x86)\PostgreSQL\16\bin\psql.exe",
    "C:\Program Files (x86)\PostgreSQL\15\bin\psql.exe",
    "C:\Program Files (x86)\PostgreSQL\14\bin\psql.exe"
)

$psqlPath = $null
foreach ($path in $pgPaths) {
    if (Test-Path $path) {
        $psqlPath = $path
        Write-Host "Found PostgreSQL at: $path" -ForegroundColor Green
        break
    }
}

if (-not $psqlPath) {
    Write-Host "PostgreSQL psql.exe not found in common locations." -ForegroundColor Red
    Write-Host "Please provide the full path to psql.exe:" -ForegroundColor Yellow
    $psqlPath = Read-Host "Path to psql.exe"
    
    if (-not (Test-Path $psqlPath)) {
        Write-Host "Invalid path. Exiting." -ForegroundColor Red
        exit 1
    }
}

# Set password
$env:PGPASSWORD = "1234"

Write-Host "Ensuring user with id=1 exists..." -ForegroundColor Cyan

# SQL to ensure user with id=1 exists
$sql = @"
-- First, try to insert user with id=1 if it doesn't exist
DO `$`$
BEGIN
    -- Check if user with id=1 exists
    IF NOT EXISTS (SELECT 1 FROM users WHERE id = 1) THEN
        -- Check if there's an existing user we can update
        IF EXISTS (SELECT 1 FROM users WHERE username = 'testuser') THEN
            -- Update existing user to have id=1 (if possible)
            UPDATE users SET id = 1 WHERE username = 'testuser' AND id != 1;
        ELSE
            -- Insert new user with id=1
            INSERT INTO users (id, username, email, password_hash) 
            VALUES (1, 'testuser', 'test@example.com', '$2b$10`$example_hash_replace_in_production');
        END IF;
    END IF;
END `$`$;

-- Verify user exists
SELECT id, username, email FROM users WHERE id = 1;
"@

# Write SQL to temp file
$tempFile = [System.IO.Path]::GetTempFileName()
$sql | Out-File -FilePath $tempFile -Encoding UTF8

try {
    # Execute SQL
    $result = & $psqlPath -U postgres -d "Lala store" -f $tempFile 2>&1
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host "✅ User with id=1 now exists!" -ForegroundColor Green
        Write-Host ""
        Write-Host "User details:" -ForegroundColor Cyan
        $result | Select-String -Pattern "^\s+\d+\s+\|" | ForEach-Object { Write-Host $_.Line }
    } else {
        Write-Host "⚠️  SQL execution completed with warnings:" -ForegroundColor Yellow
        $result | ForEach-Object { Write-Host $_ }
    }
    
    # Verify products exist
    Write-Host ""
    Write-Host "Checking products..." -ForegroundColor Cyan
    $productCount = & $psqlPath -U postgres -d "Lala store" -t -c "SELECT COUNT(*) FROM products;" 2>&1 | ForEach-Object { $_.ToString().Trim() }
    
    if ($productCount -match '^\d+$' -and [int]$productCount -gt 0) {
        Write-Host "✅ Found $productCount products" -ForegroundColor Green
    } else {
        Write-Host "⚠️  No products found. Run: $psqlPath -U postgres -d `"Lala store`" -f database\schema.sql" -ForegroundColor Yellow
    }
    
} catch {
    Write-Host "❌ Error executing SQL: $_" -ForegroundColor Red
    exit 1
} finally {
    # Clean up temp file
    Remove-Item $tempFile -ErrorAction SilentlyContinue
    $env:PGPASSWORD = ""
}

Write-Host ""
Write-Host "✅ Database fix completed!" -ForegroundColor Green
Write-Host ""
Write-Host "Next steps:" -ForegroundColor Cyan
Write-Host "1. Rebuild the backend to apply code changes:" -ForegroundColor White
Write-Host "   cd backend\build" -ForegroundColor Gray
Write-Host "   cmake --build . --config Release" -ForegroundColor Gray
Write-Host ""
Write-Host "2. Restart the backend server" -ForegroundColor White
Write-Host ""
Write-Host "3. Try adding items to cart again" -ForegroundColor White
