#ifndef NICHT_COUNT_MIN_H
#define NICHT_COUNT_MIN_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define NICHT_CM_DEPTH 4

typedef struct {
    uint32_t *table;
    size_t width;
} nicht_cm_t;

static inline nicht_cm_t* nicht_cm_create(size_t width) {
    if (width > SIZE_MAX / (NICHT_CM_DEPTH * sizeof(uint32_t))) return NULL;

    nicht_cm_t *cm = (nicht_cm_t*)malloc(sizeof(nicht_cm_t));
    if (!cm) return NULL;
    cm->width = width;
    
    if (posix_memalign((void**)&cm->table, 64, NICHT_CM_DEPTH * width * sizeof(uint32_t)) != 0) {
        free(cm);
        return NULL;
    }
    memset(cm->table, 0, NICHT_CM_DEPTH * width * sizeof(uint32_t));
    return cm;
}

// Fast inline Hash Function (Murmur-inspired mix)
static inline uint32_t nicht_cm_hash(uint64_t key, uint32_t seed) {
    key ^= seed;
    key = (key ^ (key >> 30)) * 0xbf58476d1ce4e5b9ULL;
    key = (key ^ (key >> 27)) * 0x94d049bb133111ebULL;
    return (uint32_t)(key ^ (key >> 31));
}

static inline void nicht_cm_add(nicht_cm_t *cm, uint64_t item) {
    for (uint32_t row = 0; row < NICHT_CM_DEPTH; row++) {
        uint32_t col = nicht_cm_hash(item, row * 0x9e3779b9U) % cm->width;
        cm->table[row * cm->width + col]++;
    }
}

static inline uint32_t nicht_cm_estimate(const nicht_cm_t *cm, uint64_t item) {
    uint32_t min_val = UINT32_MAX;
    for (uint32_t row = 0; row < NICHT_CM_DEPTH; row++) {
        uint32_t col = nicht_cm_hash(item, row * 0x9e3779b9U) % cm->width;
        uint32_t val = cm->table[row * cm->width + col];
        if (val < min_val) min_val = val;
    }
    return min_val;
}

static inline void nicht_cm_free(nicht_cm_t *cm) {
    free(cm->table);
    free(cm);
}

#endif
