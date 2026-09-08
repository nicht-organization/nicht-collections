#ifndef NICHT_SPECTRAL_INDEX_H
#define NICHT_SPECTRAL_INDEX_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <math.h>

#define MAX_K 8

typedef struct alignas(64) {
    uint64_t graph_id;
    uint32_t node_count;
    float spectrum[MAX_K];
} SpectralSignature;

typedef struct {
    SpectralSignature *entries;
    size_t capacity;
    size_t count;
} TSPMetricIndex;

static inline void tsp_index_init(TSPMetricIndex *idx, SpectralSignature *buffer, size_t capacity) {
    idx->entries = buffer;
    idx->capacity = capacity;
    idx->count = 0;
}

static inline bool tsp_index_insert(TSPMetricIndex *idx, uint64_t graph_id, uint32_t node_count, const float spectrum[MAX_K]) {
    if (idx->count >= idx->capacity) return false;
    SpectralSignature *sig = &idx->entries[idx->count++];
    sig->graph_id = graph_id;
    sig->node_count = node_count;
    for (size_t i = 0; i < MAX_K; i++) sig->spectrum[i] = spectrum[i];
    return true;
}

static inline int64_t tsp_index_query_nearest(const TSPMetricIndex *idx, const float query_spectrum[MAX_K], float *out_friction) {
    if (idx->count == 0) {
        *out_friction = -1.0f;
        return -1;
    }
    int64_t best_id = -1;
    float min_friction = 1e30f;
    for (size_t i = 0; i < idx->count; i++) {
        const SpectralSignature *sig = &idx->entries[i];
        float sum_sq = 0.0f;
        for (size_t k = 0; k < MAX_K; k++) {
            float diff = sig->spectrum[k] - query_spectrum[k];
            sum_sq += diff * diff;
        }
        float W = sqrtf(sum_sq);
        if (W < min_friction) {
            min_friction = W;
            best_id = (int64_t)sig->graph_id;
        }
    }
    *out_friction = min_friction;
    return best_id;
}

#endif /* NICHT_SPECTRAL_INDEX_H */
