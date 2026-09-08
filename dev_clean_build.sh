#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "=== [1/3] Cleaning build directory ==="
make clean

echo "=== [2/3] Building & Running Unit Tests ==="
make test

echo "=== [3/3] Building & Running Benchmarks ==="
make bench

echo "=== BUILD, TEST & BENCH SUITE COMPLETE (GREEN) ==="
