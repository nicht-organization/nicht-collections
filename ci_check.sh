#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "=== [1/3] Cleaning Build Directory ==="
make clean

echo "=== [2/3] Compiling & Running Unit Tests ==="
make test

echo "=== [3/3] Compiling & Executing Production Benchmarks ==="
make bench

echo "=== ALL CI CHECKS PASSED GREEN ==="
