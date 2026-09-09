#ifndef NICHT_VEB_LAYOUT_H
#define NICHT_VEB_LAYOUT_H

#include <stdint.h>
#include <stdlib.h>

typedef struct {
    int64_t *tree;
    size_t capacity;
} nicht_veb_t;

static inline nicht_veb_t* nicht_veb_create(size_t height) {
    nicht_veb_t *veb = (nicht_veb_t*)malloc(sizeof(nicht_veb_t));
    veb->capacity = (1ULL << height) - 1;
    posix_memalign((void**)&veb->tree, 64, veb->capacity * sizeof(int64_t));
    
    for (size_t i = 0; i < veb->capacity; i++) {
        veb->tree[i] = -1; // Unset sentinel
    }
    return veb;
}

static inline void nicht_veb_free(nicht_veb_t *veb) {
    free(veb->tree);
    free(veb);
}

#endif
