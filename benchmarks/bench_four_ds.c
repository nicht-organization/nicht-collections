#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>

#include "nicht/collections.h"

#define NUM_OPS_LARGE 10000000 // 10 Million
#define NUM_OPS_MEDIUM  5000000 // 5 Million

static inline double get_time_sec(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

// -----------------------------------------------------------------------------
// 1. Benchmark: Succinct Bit-Vector
// -----------------------------------------------------------------------------
static void bench_succinct_bv(void) {
    printf("=== Benchmark: Succinct Bit-Vector (Rank O(1)) ===\n");
    printf("Operations: %d bits set | %d rank queries\n", NUM_OPS_LARGE, NUM_OPS_LARGE);

    nicht_bitvector_t *bv = nicht_bv_create(NUM_OPS_LARGE);
    
    // Populate bits
    double t0 = get_time_sec();
    for (size_t i = 0; i < NUM_OPS_LARGE; i += 3) {
        nicht_bv_set(bv, i);
    }
    double t_set = get_time_sec() - t0;

    // Index Building
    t0 = get_time_sec();
    nicht_bv_build_index(bv);
    double t_index = get_time_sec() - t0;

    // Rank Queries
    t0 = get_time_sec();
    volatile size_t dummy_rank = 0;
    for (size_t i = 0; i < NUM_OPS_LARGE; i++) {
        dummy_rank += nicht_bv_rank1(bv, i);
    }
    double t_rank = get_time_sec() - t0;

    printf("Set Throughput  : %.2f M ops/sec (Time: %.4f s)\n", (NUM_OPS_LARGE / t_set) / 1e6, t_set);
    printf("Index Build Time: %.6f s\n", t_index);
    printf("Rank Throughput : %.2f M queries/sec (Time: %.4f s)\n\n", (NUM_OPS_LARGE / t_rank) / 1e6, t_rank);

    nicht_bv_free(bv);
}

// -----------------------------------------------------------------------------
// 2. Benchmark: Flat-Array Union-Find
// -----------------------------------------------------------------------------
static void bench_disjoint_set(void) {
    printf("=== Benchmark: Flat-Array Union-Find (Path Compression) ===\n");
    printf("Operations: %d elements | %d unions + finds\n", NUM_OPS_MEDIUM, NUM_OPS_MEDIUM);

    nicht_dset_t *ds = nicht_dset_create(NUM_OPS_MEDIUM);

    // Interleaved Union Operations
    double t0 = get_time_sec();
    for (uint32_t i = 0; i < NUM_OPS_MEDIUM - 1; i += 2) {
        nicht_dset_union(ds, i, i + 1);
    }
    double t_union = get_time_sec() - t0;

    // Find Queries with Path Compression
    t0 = get_time_sec();
    volatile uint32_t dummy_root = 0;
    for (uint32_t i = 0; i < NUM_OPS_MEDIUM; i++) {
        dummy_root += nicht_dset_find(ds, i);
    }
    double t_find = get_time_sec() - t0;

    printf("Union Throughput: %.2f M ops/sec (Time: %.4f s)\n", (NUM_OPS_MEDIUM / t_union) / 1e6, t_union);
    printf("Find Throughput : %.2f M ops/sec (Time: %.4f s)\n\n", (NUM_OPS_MEDIUM / t_find) / 1e6, t_find);

    nicht_dset_free(ds);
}

// -----------------------------------------------------------------------------
// 3. Benchmark: Cache-Oblivious vEB Layout
// -----------------------------------------------------------------------------
static void bench_veb_layout(void) {
    printf("=== Benchmark: Cache-Oblivious vEB Layout Tree ===\n");
    size_t height = 24; // ~16.7 million contiguous tree nodes
    size_t nodes = (1ULL << height) - 1;
    printf("Tree Depth: %zu (%zu nodes pre-allocated)\n", height, nodes);

    nicht_veb_t *veb = nicht_veb_create(height);

    double t0 = get_time_sec();
    for (size_t i = 0; i < nodes; i += 64) {
        veb->tree[i] = (int64_t)i;
    }
    double t_write = get_time_sec() - t0;

    printf("Cache Strided Access: %.2f M writes/sec (Time: %.4f s)\n\n", ((nodes / 64) / t_write) / 1e6, t_write);

    nicht_veb_free(veb);
}

// -----------------------------------------------------------------------------
// 4. Benchmark: Zero-Alloc Count-Min Sketch
// -----------------------------------------------------------------------------
static void bench_count_min(void) {
    printf("=== Benchmark: Zero-Alloc Count-Min Sketch ===\n");
    printf("Operations: %d insertions + %d estimates\n", NUM_OPS_LARGE, NUM_OPS_LARGE);

    nicht_cm_t *cm = nicht_cm_create(65536); // 64K slots per hash row

    // Insertion Throughput
    double t0 = get_time_sec();
    for (uint64_t i = 0; i < NUM_OPS_LARGE; i++) {
        nicht_cm_add(cm, i * 0x9e3779b97f4a7c15ULL);
    }
    double t_add = get_time_sec() - t0;

    // Estimate Query Throughput
    t0 = get_time_sec();
    volatile uint32_t dummy_est = 0;
    for (uint64_t i = 0; i < NUM_OPS_LARGE; i++) {
        dummy_est += nicht_cm_estimate(cm, i * 0x9e3779b97f4a7c15ULL);
    }
    double t_est = get_time_sec() - t0;

    printf("Add Throughput     : %.2f M ops/sec (Time: %.4f s)\n", (NUM_OPS_LARGE / t_add) / 1e6, t_add);
    printf("Estimate Throughput: %.2f M ops/sec (Time: %.4f s)\n\n", (NUM_OPS_LARGE / t_est) / 1e6, t_est);

    nicht_cm_free(cm);
}

// -----------------------------------------------------------------------------
// Runner Main
// -----------------------------------------------------------------------------
int main(void) {
    printf("=========================================================\n");
    printf("      EXECUTING NICHT-COLLECTIONS BENCHMARK SUITE       \n");
    printf("=========================================================\n\n");

    bench_succinct_bv();
    bench_disjoint_set();
    bench_veb_layout();
    bench_count_min();

    printf("=========================================================\n");
    printf("            BENCHMARK RUN COMPLETE (SUCCESS)            \n");
    printf("=========================================================\n");
    return 0;
}
