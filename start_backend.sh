#!/bin/bash
# LALA STORE - Start Backend (macOS / Linux)
# Tries build_mingw (optional) then build. Uses SQLite: database/lala-store.db

set -e
cd "$(dirname "$0")"

echo "Starting LALA STORE Backend..."
echo ""

# Run from project root so database path resolves
ROOT="$(pwd)"
EXECUTABLE=""
RUN_DIR="$ROOT"

if [ -f "backend/build/lala_store" ]; then
  EXECUTABLE="backend/build/lala_store"
  RUN_DIR="backend/build"
elif [ -f "backend/build/Release/lala_store" ]; then
  EXECUTABLE="backend/build/Release/lala_store"
  RUN_DIR="backend/build"
fi

if [ -z "$EXECUTABLE" ]; then
  echo "❌ No backend executable found. Build first:"
  echo "   cd backend && cmake -B build && cmake --build build"
  exit 1
fi

echo "✅ Database: database/lala-store.db (SQLite)"
echo "🚀 Starting backend on http://localhost:8001"
echo ""

cd "$ROOT/$RUN_DIR"
exec ./$(basename "$EXECUTABLE")
