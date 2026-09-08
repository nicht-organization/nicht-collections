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

## License
This project is released into the public domain under [The Unlicense][LICENSE].