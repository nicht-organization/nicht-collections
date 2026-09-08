#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include "nicht/phi_heap.h"

#define NUM_OPS 1000000

static double get_time_sec(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

int main(void) {
    printf("=== Benchmark: Phi-Heap Priority Queue ===\n");
    printf("Operations: %d inserts + %d extractions\n", NUM_OPS, NUM_OPS);

    PhiHeap *heap = phi_heap_create(NUM_OPS);
    if (!heap) {
        fprintf(stderr, "Allocation failed\n");
        return 1;
    }

    /* Seed deterministic pseudo-random sequence */
    srand(1337);

    double t0 = get_time_sec();
    for (uint32_t i = 0; i < NUM_OPS; i++) {
        uint64_t k = rand();
        phi_heap_insert(heap, k, i);
    }
    double t_insert = get_time_sec() - t0;

    t0 = get_time_sec();
    PhiElement out;
    for (uint32_t i = 0; i < NUM_OPS; i++) {
        phi_heap_extract_min(heap, &out);
    }
    double t_extract = get_time_sec() - t0;

    printf("Insert Throughput : %.2f M ops/sec (Time: %.4f s)\n",
           (NUM_OPS / t_insert) / 1e6, t_insert);
    printf("Extract Throughput: %.2f M ops/sec (Time: %.4f s)\n",
           (NUM_OPS / t_extract) / 1e6, t_extract);

    phi_heap_destroy(heap);
    return 0;
}
