#!/bin/bash

# LALA STORE - macOS Setup Script
# This script sets up the entire project on macOS

set -e  # Exit on error

echo "=========================================="
echo "  LALA STORE - macOS Setup Script"
echo "=========================================="
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print colored output
print_success() {
    echo -e "${GREEN}✅ $1${NC}"
}

print_error() {
    echo -e "${RED}❌ $1${NC}"
}

print_warning() {
    echo -e "${YELLOW}⚠️  $1${NC}"
}

print_info() {
    echo -e "ℹ️  $1"
}

# Check if running on macOS
if [[ "$OSTYPE" != "darwin"* ]]; then
    print_error "This script is for macOS only. Use setup_windows.bat for Windows."
    exit 1
fi

# Step 1: Check and install Homebrew
echo "Step 1: Checking Homebrew..."
if ! command -v brew &> /dev/null; then
    print_warning "Homebrew not found. Installing Homebrew..."
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    print_success "Homebrew installed"
else
    print_success "Homebrew is installed"
fi

# Step 2: Install PostgreSQL
echo ""
echo "Step 2: Checking PostgreSQL..."
# Ensure Homebrew PostgreSQL binaries are on PATH
if ! command -v psql &> /dev/null; then
    for PG_PREFIX in "/usr/local/opt/postgresql@15" "/usr/local/opt/postgresql@14" "/opt/homebrew/opt/postgresql@15" "/opt/homebrew/opt/postgresql@14"; do
        if [ -d "$PG_PREFIX/bin" ]; then
            export PATH="$PG_PREFIX/bin:$PATH"
            break
        fi
    done
fi

if ! command -v psql &> /dev/null; then
    print_warning "PostgreSQL not found. Installing PostgreSQL..."
    brew install postgresql@15
    brew services start postgresql@15
    print_success "PostgreSQL installed and started"
else
    print_success "PostgreSQL is installed"
    # Check if PostgreSQL is running
    if pg_isready -U postgres &> /dev/null; then
        print_success "PostgreSQL is running"
    else
        print_warning "Starting PostgreSQL service..."
        brew services start postgresql@15 || brew services start postgresql
        sleep 3
        if pg_isready -U postgres &> /dev/null; then
            print_success "PostgreSQL started"
        else
            print_error "Failed to start PostgreSQL. Please start it manually."
            exit 1
        fi
    fi
fi

# Step 3: Install Node.js and npm
echo ""
echo "Step 3: Checking Node.js..."
if ! command -v node &> /dev/null; then
    print_warning "Node.js not found. Installing Node.js..."
    brew install node
    print_success "Node.js installed"
else
    NODE_VERSION=$(node --version)
    print_success "Node.js is installed ($NODE_VERSION)"
fi

if ! command -v npm &> /dev/null; then
    print_error "npm not found"
    exit 1
else
    NPM_VERSION=$(npm --version)
    print_success "npm is installed ($NPM_VERSION)"
fi

# Step 4: Install CMake and C++ build tools
echo ""
echo "Step 4: Checking CMake and C++ tools..."
if ! command -v cmake &> /dev/null; then
    print_warning "CMake not found. Installing CMake..."
    brew install cmake
    print_success "CMake installed"
else
    CMAKE_VERSION=$(cmake --version | head -n1)
    print_success "CMake is installed ($CMAKE_VERSION)"
fi

if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
    print_warning "C++ compiler not found. Installing Xcode Command Line Tools..."
    xcode-select --install || true
    print_success "Xcode Command Line Tools installation initiated"
else
    print_success "C++ compiler is available"
fi

# Step 5: Setup Database
echo ""
echo "Step 5: Setting up database..."
export PGPASSWORD=1234

# Check if database exists
if psql -U postgres -lqt 2>/dev/null | cut -d \| -f 1 | grep -qw "Lala store"; then
    print_warning "Database 'Lala store' already exists."
    read -p "Do you want to drop and recreate it? (y/N): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        print_info "Dropping existing database..."
        psql -U postgres -c 'DROP DATABASE "Lala store";' 2>/dev/null || true
    else
        print_info "Using existing database..."
    fi
fi

# Create database if it doesn't exist
if ! psql -U postgres -lqt 2>/dev/null | cut -d \| -f 1 | grep -qw "Lala store"; then
    print_info "Creating database 'Lala store'..."
    if psql -U postgres -c 'CREATE DATABASE "Lala store";' 2>/dev/null; then
        print_success "Database created successfully"
    else
        print_error "Failed to create database. Please check PostgreSQL credentials."
        print_info "Trying to set password for postgres user..."
        psql -U postgres -c "ALTER USER postgres WITH PASSWORD '1234';" 2>/dev/null || true
        psql -U postgres -c 'CREATE DATABASE "Lala store";' 2>/dev/null
        if [ $? -eq 0 ]; then
            print_success "Database created successfully"
        else
            print_error "Failed to create database. Please check PostgreSQL is running and credentials are correct."
            exit 1
        fi
    fi
fi

# Run schema
print_info "Running database schema..."
if psql -U postgres -d "Lala store" -f database/schema.sql 2>/dev/null; then
    print_success "Database schema loaded successfully"
else
    print_error "Failed to load schema. Please check database/schema.sql exists."
    exit 1
fi

unset PGPASSWORD

# Step 6: Download Crow Framework
echo ""
echo "Step 6: Downloading Crow framework..."
if [ ! -d "backend/crow" ]; then
    print_info "Cloning Crow framework..."
    cd backend
    git clone https://github.com/CrowCpp/Crow.git crow 2>/dev/null || {
        print_error "Failed to clone Crow. Please check internet connection."
        exit 1
    }
    cd ..
    print_success "Crow framework downloaded"
else
    print_success "Crow framework already exists"
fi

# Step 7: Download nlohmann/json
echo ""
echo "Step 7: Downloading nlohmann/json..."
if [ ! -f "backend/include/nlohmann/json.hpp" ]; then
    print_info "Downloading json.hpp..."
    mkdir -p backend/include/nlohmann
    curl -L -o backend/include/nlohmann/json.hpp https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp 2>/dev/null || {
        print_error "Failed to download json.hpp. Please check internet connection."
        exit 1
    }
    print_success "nlohmann/json downloaded"
else
    print_success "nlohmann/json already exists"
fi

# Step 8: Build Backend
echo ""
echo "Step 8: Building backend..."
cd backend
if [ ! -d "build" ]; then
    mkdir build
fi
cd build

print_info "Running CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release 2>&1 | tee cmake_output.log
if [ ${PIPESTATUS[0]} -ne 0 ]; then
    print_error "CMake configuration failed. Check cmake_output.log for details."
    exit 1
fi

print_info "Building project..."
make 2>&1 | tee build_output.log
if [ ${PIPESTATUS[0]} -ne 0 ]; then
    print_error "Build failed. Check build_output.log for details."
    exit 1
fi

if [ -f "lala_store" ]; then
    print_success "Backend built successfully"
else
    print_error "Backend executable not found"
    exit 1
fi

cd ../..

# Step 9: Setup Frontend
echo ""
echo "Step 9: Setting up frontend..."
cd frontend

if [ ! -d "node_modules" ]; then
    print_info "Installing npm dependencies..."
    npm install 2>&1 | tee npm_install.log
    if [ ${PIPESTATUS[0]} -ne 0 ]; then
        print_error "npm install failed. Check npm_install.log for details."
        exit 1
    fi
    print_success "Frontend dependencies installed"
else
    print_success "Frontend dependencies already installed"
fi

cd ..

# Step 10: Test Database Connection
echo ""
echo "Step 10: Testing database connection..."
export PGPASSWORD=1234
if psql -U postgres -d "Lala store" -c "SELECT COUNT(*) FROM products;" > /dev/null 2>&1; then
    PRODUCT_COUNT=$(psql -U postgres -d "Lala store" -t -c "SELECT COUNT(*) FROM products;" 2>/dev/null | xargs)
    print_success "Database connection successful (Found $PRODUCT_COUNT products)"
else
    print_error "Database connection test failed"
    exit 1
fi
unset PGPASSWORD

# Step 11: Verify Configuration
echo ""
echo "Step 11: Verifying configuration..."
if [ -f "backend/config/db_config.json" ]; then
    print_success "Database configuration file exists"
    # Verify config matches
    DB_NAME=$(grep -o '"database": "[^"]*"' backend/config/db_config.json | cut -d'"' -f4)
    if [ "$DB_NAME" = "Lala store" ]; then
        print_success "Database name in config is correct"
    else
        print_warning "Database name mismatch in config"
    fi
else
    print_error "Database configuration file not found"
    exit 1
fi

# Final Summary
echo ""
echo "=========================================="
echo "  Setup Complete!"
echo "=========================================="
echo ""
print_success "All dependencies installed"
print_success "Database configured and connected"
print_success "Backend built successfully"
print_success "Frontend dependencies installed"
echo ""
echo "To start the application:"
echo ""
echo "1. Start Backend (Terminal 1):"
echo "   cd backend/build"
echo "   ./lala_store"
echo ""
echo "2. Start Frontend (Terminal 2):"
echo "   cd frontend"
echo "   npm run dev"
echo ""
echo "3. Open browser:"
echo "   http://localhost:3001"
echo ""
echo "Database Info:"
echo "  Name: Lala store"
echo "  User: postgres"
echo "  Password: 1234"
echo "  Host: localhost"
echo "  Port: 5432"
echo ""
