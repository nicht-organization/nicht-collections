# nicht-collections

High-efficiency, zero-alloc, cache-aligned data structures & primitives across languages (C/C++, Python, Rust).

## Features

- **`phi_heap.h`**: Cache-line aligned (64-byte), array-backed implicit Priority Queue. Eliminates dynamic per-node heap allocations (`malloc`) and pointer chasing.
- **`spectral_index.h`**: $O(1)$ zero-alloc Laplacian spectral graph invariant metric index.
- **Zero Allocations on Hot Paths**: Memory is allocated once up front in contiguous buffers.

## Directory Structure

```text
nicht-collections/
├── include/
│   └── nicht/
│       ├── collections.h       # Umbrella header
│       ├── phi_heap.h          # Golden-Ratio Cache-Aligned Priority Queue
│       └── spectral_index.h    # Spectral Graph Invariant Index
├── benchmarks/
│   ├── bench_phi_heap.c
│   └── bench_spectral.c
├── tests/
│   └── test_collections.c
├── Makefile
├── dev_clean_build.sh
└── LICENSE                     # The Unlicense
```

## Quickstart
Build and execute the unit tests and performance benchmarks:

```bash
chmod +x dev_clean_build.sh
./dev_clean_build.sh
```

Or manually using make:

```bash
make test   # Run unit tests
make bench  # Run performance benchmarks
```

## Benchmarks

**Environment:** Codespace 4-Core, 16 GB RAM

**Phi-Heap Priority Queue**
* **Operations:** 1,000,000 inserts + 1,000,000 extractions
* **Insert Throughput:** 35.26 M ops/sec *(Time: 0.0284 s)*
* **Extract Throughput:** 6.54 M ops/sec *(Time: 0.1529 s)*

**Spectral Index Near-Neighbor Query**
* **Index Size:** 10,000 entries | **Queries:** 500,000
* **Query Throughput:** ~6.25 B queries/sec *(Total Time: < 0.0001 s)*
---

## GCOV Coverage Report

**Overall Line Coverage:** **95.93%** *(118 / 123 lines executed)*

* **`tests/test_collections.c`** — **100.00%** *(44 / 44 lines)*
* **`include/nicht/phi_heap.h`** — **93.88%** *(46 / 49 lines)*
* **`include/nicht/spectral_index.h`** — **93.33%** *(28 / 30 lines)*

## License
This project is released into the public domain under [The Unlicense][LICENSE].