#!/bin/bash

# LALA STORE - Connection Test Script (macOS/Linux)
# Tests database, backend, and frontend connections

echo "=========================================="
echo "  LALA STORE - Connection Test"
echo "=========================================="
echo ""

# Ensure PostgreSQL tools are available (Homebrew)
if ! command -v psql > /dev/null 2>&1; then
  for PG_PREFIX in "/usr/local/opt/postgresql@15" "/usr/local/opt/postgresql@14" "/opt/homebrew/opt/postgresql@15" "/opt/homebrew/opt/postgresql@14"; do
    if [ -d "$PG_PREFIX/bin" ]; then
      export PATH="$PG_PREFIX/bin:$PATH"
      break
    fi
  done
fi

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

print_success() { echo -e "${GREEN}✅ $1${NC}"; }
print_error() { echo -e "${RED}❌ $1${NC}"; }
print_info() { echo -e "ℹ️  $1"; }

# Test 1: PostgreSQL Connection
echo "Test 1: PostgreSQL Connection"
export PGPASSWORD=1234
if psql -U postgres -d "Lala store" -c "SELECT 1;" > /dev/null 2>&1; then
    print_success "PostgreSQL connection successful"
    
    # Check tables
    TABLE_COUNT=$(psql -U postgres -d "Lala store" -t -c "SELECT COUNT(*) FROM information_schema.tables WHERE table_schema = 'public';" 2>/dev/null | xargs)
    print_info "Found $TABLE_COUNT tables in database"
    
    # Check products
    PRODUCT_COUNT=$(psql -U postgres -d "Lala store" -t -c "SELECT COUNT(*) FROM products;" 2>/dev/null | xargs)
    print_info "Found $PRODUCT_COUNT products"
else
    print_error "PostgreSQL connection failed"
    print_info "Check if PostgreSQL is running: pg_isready -U postgres"
    exit 1
fi
unset PGPASSWORD

# Test 2: Backend API
echo ""
echo "Test 2: Backend API"
if curl -s http://localhost:8001/api/home/featured > /dev/null 2>&1; then
    print_success "Backend API is responding"
    
    # Test actual endpoint
    RESPONSE=$(curl -s http://localhost:8001/api/home/featured)
    if echo "$RESPONSE" | grep -q "success"; then
        print_success "Backend API endpoint working correctly"
    else
        print_error "Backend API returned unexpected response"
    fi
else
    print_error "Backend API is not responding"
    print_info "Make sure backend is running: cd backend/build && ./lala_store"
fi

# Test 3: Frontend
echo ""
echo "Test 3: Frontend"
if curl -s http://localhost:3001 > /dev/null 2>&1; then
    print_success "Frontend is responding"
else
    print_error "Frontend is not responding"
    print_info "Make sure frontend is running: cd frontend && npm run dev"
fi

# Test 4: Database Configuration
echo ""
echo "Test 4: Database Configuration"
if [ -f "backend/config/db_config.json" ]; then
    DB_NAME=$(grep -o '"database": "[^"]*"' backend/config/db_config.json | cut -d'"' -f4)
    DB_USER=$(grep -o '"user": "[^"]*"' backend/config/db_config.json | cut -d'"' -f4)
    
    if [ "$DB_NAME" = "Lala store" ] && [ "$DB_USER" = "postgres" ]; then
        print_success "Database configuration is correct"
    else
        print_error "Database configuration mismatch"
        print_info "Expected: database='Lala store', user='postgres'"
        print_info "Found: database='$DB_NAME', user='$DB_USER'"
    fi
else
    print_error "Database configuration file not found"
fi

echo ""
echo "=========================================="
echo "  Connection Test Complete"
echo "=========================================="
