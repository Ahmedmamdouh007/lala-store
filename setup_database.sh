#!/bin/bash

# LALA STORE Database Setup Script
# This script creates the database and runs the schema

echo "LALA STORE Database Setup"
echo "========================"
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

# Check if PostgreSQL is running
if ! pg_isready -U postgres > /dev/null 2>&1; then
    echo "❌ PostgreSQL is not running. Please start PostgreSQL first."
    exit 1
fi

echo "✅ PostgreSQL is running"
echo ""

# Check if database exists
if psql -U postgres -lqt | cut -d \| -f 1 | grep -qw "Lala store"; then
    echo "⚠️  Database 'Lala store' already exists."
    read -p "Do you want to drop and recreate it? (y/N): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        echo "Dropping existing database..."
        psql -U postgres -c 'DROP DATABASE "Lala store";'
    else
        echo "Using existing database..."
    fi
fi

# Create database if it doesn't exist
if ! psql -U postgres -lqt | cut -d \| -f 1 | grep -qw "Lala store"; then
    echo "Creating database 'Lala store'..."
    psql -U postgres -c 'CREATE DATABASE "Lala store";'
    if [ $? -eq 0 ]; then
        echo "✅ Database created successfully"
    else
        echo "❌ Failed to create database"
        exit 1
    fi
fi

# Run schema
echo ""
echo "Running database schema..."
psql -U postgres -d "Lala store" -f database/schema.sql

if [ $? -eq 0 ]; then
    echo ""
    echo "✅ Database setup completed successfully!"
    echo ""
    echo "You can now start the backend server."
else
    echo ""
    echo "❌ Failed to run schema"
    exit 1
fi
