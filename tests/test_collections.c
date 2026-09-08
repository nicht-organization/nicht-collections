#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "nicht/phi_heap.h"
#include "nicht/spectral_index.h"

static void test_phi_heap_basic(void) {
    PhiHeap *heap = phi_heap_create(16);
    assert(heap != NULL);
    assert(heap->size == 0);

    /* Insert out-of-order elements */
    assert(phi_heap_insert(heap, 42, 100));
    assert(phi_heap_insert(heap, 10, 200));
    assert(phi_heap_insert(heap, 25, 300));
    assert(phi_heap_insert(heap, 5,  400));
    assert(heap->size == 4);

    /* Peek Min */
    PhiElement peeked;
    assert(phi_heap_peek_min(heap, &peeked));
    assert(peeked.key == 5);
    assert(peeked.value == 400);

    /* Extract Min sequence */
    PhiElement elem;
    assert(phi_heap_extract_min(heap, &elem));
    assert(elem.key == 5);

    assert(phi_heap_extract_min(heap, &elem));
    assert(elem.key == 10);

    assert(phi_heap_extract_min(heap, &elem));
    assert(elem.key == 25);

    assert(phi_heap_extract_min(heap, &elem));
    assert(elem.key == 42);

    assert(heap->size == 0);
    assert(!phi_heap_extract_min(heap, &elem));

    phi_heap_destroy(heap);
    printf("  [PASS] test_phi_heap_basic\n");
}

static void test_spectral_index_basic(void) {
    static SpectralSignature storage[16];
    TSPMetricIndex index;
    tsp_index_init(&index, storage, 16);

    float spec1[MAX_K] = {0.0f, 1.25f, 2.41f, 3.80f, 4.10f, 5.22f, 6.01f, 7.11f};
    float spec2[MAX_K] = {0.0f, 0.85f, 1.95f, 2.70f, 3.45f, 4.12f, 5.00f, 5.90f};

    assert(tsp_index_insert(&index, 1001, 16, spec1));
    assert(tsp_index_insert(&index, 1002, 16, spec2));

    float query[MAX_K] = {0.0f, 1.28f, 2.39f, 3.82f, 4.08f, 5.20f, 6.05f, 7.09f};
    float friction = 0.0f;
    int64_t matched_id = tsp_index_query_nearest(&index, query, &friction);

    assert(matched_id == 1001);
    assert(friction > 0.0f && friction < 0.2f);

    printf("  [PASS] test_spectral_index_basic\n");
}

int main(void) {
    printf("=== Executing nicht-collections Unit Tests ===\n");
    test_phi_heap_basic();
    test_spectral_index_basic();
    printf("=== All Unit Tests Passed Green! ===\n");
    return 0;
}
