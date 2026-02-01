#!/bin/bash

# LALA STORE - Start Frontend Script (macOS/Linux)

echo "Starting LALA STORE Frontend..."
echo ""

cd "$(dirname "$0")/frontend"

# Check if node_modules exists
if [ ! -d "node_modules" ]; then
    echo "❌ Dependencies not installed. Installing..."
    npm install
    if [ $? -ne 0 ]; then
        echo "❌ Failed to install dependencies"
        exit 1
    fi
fi

echo "🚀 Starting frontend development server on http://localhost:3001"
echo ""

npm run dev
