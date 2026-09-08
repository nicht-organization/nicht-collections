#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "nicht/spectral_index.h"

#define INDEX_CAPACITY 10000
#define NUM_QUERIES    500000

static double get_time_sec(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

int main(void) {
    printf("=== Benchmark: Spectral Index Near-Neighbor Query ===\n");
    printf("Index Size: %d entries | Queries: %d\n", INDEX_CAPACITY, NUM_QUERIES);

    SpectralSignature *storage = (SpectralSignature*)malloc(INDEX_CAPACITY * sizeof(SpectralSignature));
    TSPMetricIndex index;
    tsp_index_init(&index, storage, INDEX_CAPACITY);

    /* Populate Index */
    for (size_t i = 0; i < INDEX_CAPACITY; i++) {
        float spec[MAX_K];
        for (size_t k = 0; k < MAX_K; k++) {
            spec[k] = (float)(i + k) * 0.1f;
        }
        tsp_index_insert(&index, i + 1000, 16, spec);
    }

    float query_spec[MAX_K] = {0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f};
    float dummy_friction = 0.0f;

    double t0 = get_time_sec();
    for (int q = 0; q < NUM_QUERIES; q++) {
        query_spec[0] = (float)(q % 100) * 0.01f;
        tsp_index_query_nearest(&index, query_spec, &dummy_friction);
    }
    double elapsed = get_time_sec() - t0;

    printf("Query Throughput : %.2f K queries/sec (Total: %.4f s)\n",
           (NUM_QUERIES / elapsed) / 1e3, elapsed);

    free(storage);
    return 0;
}
