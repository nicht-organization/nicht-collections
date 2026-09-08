#ifndef NICHT_PHI_HEAP_H
#define NICHT_PHI_HEAP_H


#include <stdint.h>
#include <stdlib.h>

typedef struct {
    uint64_t key;
    uint64_t value;
} PhiElement;

typedef struct {
    PhiElement *data;
    uint32_t capacity;
    uint32_t size;
} PhiHeap;

static inline PhiHeap* phi_heap_create(uint32_t capacity) {
    PhiHeap *heap = (PhiHeap*)malloc(sizeof(PhiHeap));
    if (!heap) return NULL;

    if (posix_memalign((void**)&heap->data, 64, capacity * sizeof(PhiElement)) != 0) {
        free(heap);
        return NULL;
    }
    heap->capacity = capacity;
    heap->size = 0;
    return heap;
}

static inline void phi_heap_destroy(PhiHeap *heap) {
    if (heap) {
        if (heap->data) free(heap->data);
        free(heap);
    }
}

static inline int phi_heap_peek_min(const PhiHeap *heap, PhiElement *out) {
    if (heap->size == 0) return 0;
    *out = heap->data[0];
    return 1;
}

static inline int phi_heap_insert(PhiHeap *heap, uint64_t key, uint64_t value) {
    if (heap->size >= heap->capacity) return 0;
    uint32_t idx = heap->size++;
    heap->data[idx].key = key;
    heap->data[idx].value = value;

    while (idx > 0) {
        uint32_t parent = (idx - 1) >> 1;
        if (heap->data[parent].key <= heap->data[idx].key) break;
        PhiElement tmp = heap->data[idx];
        heap->data[idx] = heap->data[parent];
        heap->data[parent] = tmp;
        idx = parent;
    }
    return 1;
}

static inline int phi_heap_extract_min(PhiHeap *heap, PhiElement *out) {
    if (heap->size == 0) return 0;
    *out = heap->data[0];
    heap->data[0] = heap->data[--heap->size];

    uint32_t idx = 0;
    uint32_t half = heap->size >> 1;
    while (idx < half) {
        uint32_t left = (idx << 1) + 1;
        uint32_t right = left + 1;
        uint32_t smallest = left;
        if (right < heap->size && heap->data[right].key < heap->data[left].key) {
            smallest = right;
        }
        if (heap->data[idx].key <= heap->data[smallest].key) break;
        PhiElement tmp = heap->data[idx];
        heap->data[idx] = heap->data[smallest];
        heap->data[smallest] = tmp;
        idx = smallest;
    }
    return 1;
}

#endif /* NICHT_PHI_HEAP_H */
