#!/bin/bash

# LALA STORE - Start Backend Script (macOS/Linux)

echo "Starting LALA STORE Backend..."
echo ""

cd "$(dirname "$0")/backend"

# Ensure PostgreSQL tools are available (Homebrew)
if ! command -v psql > /dev/null 2>&1; then
    for PG_PREFIX in "/usr/local/opt/postgresql@15" "/usr/local/opt/postgresql@14" "/opt/homebrew/opt/postgresql@15" "/opt/homebrew/opt/postgresql@14"; do
        if [ -d "$PG_PREFIX/bin" ]; then
            export PATH="$PG_PREFIX/bin:$PATH"
            break
        fi
    done
fi

# Check if build directory exists
if [ ! -d "build" ]; then
    echo "❌ Build directory not found. Please run setup script first."
    exit 1
fi

cd build

# Check if executable exists
if [ ! -f "lala_store" ]; then
    echo "❌ Backend executable not found. Please build the project first."
    exit 1
fi

# Test database connection before starting
export PGPASSWORD=1234
if ! psql -U postgres -d "Lala store" -c "SELECT 1;" > /dev/null 2>&1; then
    echo "❌ Cannot connect to database. Please check PostgreSQL is running."
    echo "   Database: Lala store"
    echo "   User: postgres"
    echo "   Password: 1234"
    unset PGPASSWORD
    exit 1
fi
unset PGPASSWORD

echo "✅ Database connection verified"
echo "🚀 Starting backend server on http://localhost:8001"
echo ""

./lala_store
