#!/bin/bash
# LALA STORE - Build backend (macOS / Linux)
# Requires: cmake, C++ compiler (gcc/clang), SQLite3 dev headers
#   macOS:  brew install cmake sqlite3
#   Ubuntu: sudo apt install build-essential cmake libsqlite3-dev

set -e
cd "$(dirname "$0")"

echo "Building LALA STORE backend..."
echo ""

BUILD_DIR="build"
[ -n "$1" ] && BUILD_DIR="$1"

if [ -d "$BUILD_DIR" ]; then
  rm -rf "$BUILD_DIR"
fi
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j

echo ""
echo "Build succeeded. Run from project root: ./start_backend.sh"
echo ""
