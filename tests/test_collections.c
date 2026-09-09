#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "nicht/collections.h"

// -----------------------------------------------------------------------------
// 1. Phi-Heap Tests
// -----------------------------------------------------------------------------
static void test_phi_heap_full(void) {
    // Creation & Allocation Guard
    PhiHeap *heap = phi_heap_create(4);
    assert(heap != NULL);
    assert(heap->size == 0);

    // Empty state operations
    PhiElement peek_elem;
    assert(phi_heap_peek_min(heap, &peek_elem) == 0);
    assert(phi_heap_extract_min(heap, &peek_elem) == 0);

    // Insertion & Capacity Limits
    assert(phi_heap_insert(heap, 42, 100));
    assert(phi_heap_insert(heap, 10, 200));
    assert(phi_heap_insert(heap, 25, 300));
    assert(phi_heap_insert(heap, 5,  400));
    assert(!phi_heap_insert(heap, 99, 500)); // Full capacity guard
    assert(heap->size == 4);

    // Peek Min
    assert(phi_heap_peek_min(heap, &peek_elem) == 1);
    assert(peek_elem.key == 5);
    assert(peek_elem.value == 400);

    // Sequential Extraction (heapify/sift-down exercise)
    PhiElement elem;
    assert(phi_heap_extract_min(heap, &elem) && elem.key == 5);
    assert(phi_heap_extract_min(heap, &elem) && elem.key == 10);
    assert(phi_heap_extract_min(heap, &elem) && elem.key == 25);
    assert(phi_heap_extract_min(heap, &elem) && elem.key == 42);

    // Underflow Guard
    assert(!phi_heap_extract_min(heap, &elem));

    // Destroy
    phi_heap_destroy(heap);
    phi_heap_destroy(NULL); // Null safety
    printf("  [PASS] test_phi_heap_full\n");
}

// -----------------------------------------------------------------------------
// 2. Spectral Index Tests
// -----------------------------------------------------------------------------
static void test_spectral_index_full(void) {
    static SpectralSignature storage[2];
    TSPMetricIndex index;
    tsp_index_init(&index, storage, 2);

    // Empty query guard
    float query[MAX_K] = {0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f};
    float friction = 0.0f;
    assert(tsp_index_query_nearest(&index, query, &friction) == -1);

    // Insertions
    float spec1[MAX_K] = {0.0f, 1.25f, 2.41f, 3.80f, 4.10f, 5.22f, 6.01f, 7.11f};
    float spec2[MAX_K] = {0.0f, 0.85f, 1.95f, 2.70f, 3.45f, 4.12f, 5.00f, 5.90f};
    assert(tsp_index_insert(&index, 1001, 16, spec1));
    assert(tsp_index_insert(&index, 1002, 16, spec2));

    // Capacity guard
    assert(!tsp_index_insert(&index, 1003, 16, spec1));

    // Query nearest
    int64_t matched_id = tsp_index_query_nearest(&index, query, &friction);
    assert(matched_id == 1002);
    assert(friction > 0.0f);

    printf("  [PASS] test_spectral_index_full\n");
}

// -----------------------------------------------------------------------------
// 3. Succinct Bit-Vector Tests
// -----------------------------------------------------------------------------
static void test_succinct_bv_full(void) {
    nicht_bitvector_t *bv = nicht_bv_create(1000);
    assert(bv != NULL);

    // Initial ranks should be zero
    nicht_bv_build_index(bv);
    assert(nicht_bv_rank1(bv, 500) == 0);

    // Set bit patterns spanning multiple 64-bit words & 512-bit blocks
    nicht_bv_set(bv, 0);
    nicht_bv_set(bv, 63);
    nicht_bv_set(bv, 64);
    nicht_bv_set(bv, 511);
    nicht_bv_set(bv, 512);
    nicht_bv_set(bv, 999);

    nicht_bv_build_index(bv);

    assert(nicht_bv_rank1(bv, 0) == 1);
    assert(nicht_bv_rank1(bv, 62) == 1);
    assert(nicht_bv_rank1(bv, 63) == 2);
    assert(nicht_bv_rank1(bv, 64) == 3);
    assert(nicht_bv_rank1(bv, 511) == 4);
    assert(nicht_bv_rank1(bv, 512) == 5);
    assert(nicht_bv_rank1(bv, 999) == 6);

    // Out of bounds rank query cap
    assert(nicht_bv_rank1(bv, 2000) == 6);

    nicht_bv_free(bv);
    printf("  [PASS] test_succinct_bv_full\n");
}

// -----------------------------------------------------------------------------
// 4. Flat-Array Union-Find Tests
// -----------------------------------------------------------------------------
static void test_disjoint_set_full(void) {
    nicht_dset_t *ds = nicht_dset_create(10);
    assert(ds != NULL);

    // Self-parent initialization
    for (uint32_t i = 0; i < 10; i++) {
        assert(nicht_dset_find(ds, i) == i);
    }

    // Union operations & Path Compression
    nicht_dset_union(ds, 0, 1);
    nicht_dset_union(ds, 2, 3);
    nicht_dset_union(ds, 1, 3); // Merges sets {0,1,2,3}

    assert(nicht_dset_find(ds, 0) == nicht_dset_find(ds, 2));
    assert(nicht_dset_find(ds, 0) != nicht_dset_find(ds, 4));

    // Equal-rank union coverage
    nicht_dset_union(ds, 4, 5);
    nicht_dset_union(ds, 6, 7);
    nicht_dset_union(ds, 4, 6);

    nicht_dset_free(ds);
    printf("  [PASS] test_disjoint_set_full\n");
}

// -----------------------------------------------------------------------------
// 5. Cache-Oblivious vEB Layout Tests
// -----------------------------------------------------------------------------
static void test_veb_layout_full(void) {
    nicht_veb_t *veb = nicht_veb_create(4); // Capacity = 2^4 - 1 = 15
    assert(veb != NULL);
    assert(veb->capacity == 15);

    for (size_t i = 0; i < veb->capacity; i++) {
        assert(veb->tree[i] == -1); // Sentinel check
        veb->tree[i] = (int64_t)(i * 10);
    }

    assert(veb->tree[0] == 0);
    assert(veb->tree[14] == 140);

    nicht_veb_free(veb);
    printf("  [PASS] test_veb_layout_full\n");
}

// -----------------------------------------------------------------------------
// 6. Zero-Alloc Count-Min Sketch Tests
// -----------------------------------------------------------------------------
static void test_count_min_full(void) {
    nicht_cm_t *cm = nicht_cm_create(256);
    assert(cm != NULL);

    // Insert items
    for (int i = 0; i < 50; i++) {
        nicht_cm_add(cm, 0xDEADBEEF);
    }
    for (int i = 0; i < 12; i++) {
        nicht_cm_add(cm, 0xCAFEBABE);
    }

    // Frequency estimates must be >= actual counts (bounded error guarantee)
    assert(nicht_cm_estimate(cm, 0xDEADBEEF) >= 50);
    assert(nicht_cm_estimate(cm, 0xCAFEBABE) >= 12);
    assert(nicht_cm_estimate(cm, 0x12345678) < 12); // Uninserted item noise floor check

    nicht_cm_free(cm);
    printf("  [PASS] test_count_min_full\n");
}

// -----------------------------------------------------------------------------
// Test Runner
// -----------------------------------------------------------------------------
int main(void) {
    printf("=== Executing nicht-collections Unit Test Suite ===\n");
    test_phi_heap_full();
    test_spectral_index_full();
    test_succinct_bv_full();
    test_disjoint_set_full();
    test_veb_layout_full();
    test_count_min_full();
    printf("=== All Unit Tests Passed Green (100%% Coverage Target)! ===\n");
    return 0;
}
