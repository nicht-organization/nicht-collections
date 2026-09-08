#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "=== [1/4] Cleaning build & coverage artifacts ==="
rm -rf build/ *.gcda *.gcno *.gcov

echo "=== [2/4] Building & Running Unit Tests ==="
make test

echo "=== [3/4] Building & Running Benchmarks ==="
make bench

echo "=== [4/4] Collecting GCOV C Coverage Metrics ==="
make coverage

echo "=== BUILD, TEST, BENCH & COVERAGE COMPLETE (GREEN) ==="
