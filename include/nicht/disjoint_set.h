#ifndef NICHT_DISJOINT_SET_H
#define NICHT_DISJOINT_SET_H

#include <stdint.h>
#include <stdlib.h>

typedef struct {
    uint32_t *parent;
    uint8_t  *rank;
    size_t capacity;
} nicht_dset_t;
static inline nicht_dset_t* nicht_dset_create(size_t capacity) {
    if (capacity > SIZE_MAX / sizeof(uint32_t)) return NULL;

    nicht_dset_t *ds = (nicht_dset_t*)malloc(sizeof(nicht_dset_t));
    if (!ds) return NULL;
    ds->capacity = capacity;
    
    if (posix_memalign((void**)&ds->parent, 64, capacity * sizeof(uint32_t)) != 0 ||
        posix_memalign((void**)&ds->rank, 64, capacity * sizeof(uint8_t)) != 0) {
        free(ds);
        return NULL;
    }

    for (size_t i = 0; i < capacity; i++) {
        ds->parent[i] = (uint32_t)i;
        ds->rank[i] = 0;
    }
    return ds;
}

// Path compression find
static inline uint32_t nicht_dset_find(nicht_dset_t *ds, uint32_t i) {
    uint32_t root = i;
    while (root != ds->parent[root]) {
        root = ds->parent[root];
    }
    // Path halving/compression
    uint32_t curr = i;
    while (curr != root) {
        uint32_t nxt = ds->parent[curr];
        ds->parent[curr] = root;
        curr = nxt;
    }
    return root;
}

// Union by rank
static inline void nicht_dset_union(nicht_dset_t *ds, uint32_t i, uint32_t j) {
    uint32_t root_i = nicht_dset_find(ds, i);
    uint32_t root_j = nicht_dset_find(ds, j);

    if (root_i != root_j) {
        if (ds->rank[root_i] < ds->rank[root_j]) {
            ds->parent[root_i] = root_j;
        } else if (ds->rank[root_i] > ds->rank[root_j]) {
            ds->parent[root_j] = root_i;
        } else {
            ds->parent[root_j] = root_i;
            ds->rank[root_i]++;
        }
    }
}

static inline void nicht_dset_free(nicht_dset_t *ds) {
    free(ds->parent);
    free(ds->rank);
    free(ds);
}

#endif
