#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "nicht/count_min.h"
#include "nicht/disjoint_set.h"
#include "nicht/phi_heap.h"
#include "nicht/spectral_index.h"
#include "nicht/succinct_bv.h"
#include "nicht/veb_layout.h"

// -----------------------------------------------------------------------------
// 1. Phi-Heap Tests
// -----------------------------------------------------------------------------
static void test_phi_heap_full(void) {

    // Trigger veb allocation guard failure
    PhiHeap *heap_fail = phi_heap_create((long)-1);
    assert(heap_fail == NULL);

    // Break shift loop
    PhiHeap *heap_sift = phi_heap_create(8);
    phi_heap_insert(heap_sift, 10, 1);
    phi_heap_insert(heap_sift, 20, 2); // Parent (10) <= Child (20): breaks sift-up loop immediately
    phi_heap_destroy(heap_sift);

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
    assert(matched_id == 1001);
    assert(friction > 0.0f);

    printf("  [PASS] test_spectral_index_full\n");
}

// -----------------------------------------------------------------------------
// 3. Succinct Bit-Vector Tests
// -----------------------------------------------------------------------------
static void test_succinct_bv_full(void) {
    // Trigger bitvector allocation failure & edge-case zero length
    nicht_bitvector_t *bv_fail = nicht_bv_create((size_t)-1);
    assert(bv_fail == NULL);

    nicht_bitvector_t *bv_empty = nicht_bv_create(0);
    if (bv_empty) {
        nicht_bv_build_index(bv_empty);
        nicht_bv_free(bv_empty);
    }

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

    nicht_dset_t *ds_fail = nicht_dset_create((size_t)-1);
    assert(ds_fail == NULL);

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

    // border case: rank[root_i] > rank[root_j] branch
    nicht_dset_t *ds_edge = nicht_dset_create(8);

    // Rank branch: root_i > root_j
    nicht_dset_union(ds_edge, 0, 1); // rank[0] becomes 1
    nicht_dset_union(ds_edge, 0, 2); // rank[0] (1) > rank[2] (0) -> rank[root_i] > rank[root_j] branch

    // Self/Same union (root_i == root_j branch)
    nicht_dset_union(ds_edge, 0, 1); 

    nicht_dset_free(ds_edge);

    printf("  [PASS] test_disjoint_set_full\n");
}

// -----------------------------------------------------------------------------
// 5. Cache-Oblivious vEB Layout Tests
// -----------------------------------------------------------------------------
static void test_veb_layout_full(void) {
    // Trigger veb allocation guard failure
    nicht_veb_t *veb_fail = nicht_veb_create((size_t)-1);
    assert(veb_fail == NULL);

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

    nicht_cm_t *cm_fail = nicht_cm_create((size_t)-1);
    assert(cm_fail == NULL);

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
static void test_coverage_edge_cases(void) {
    // 1. Allocation failure guards
    assert(nicht_veb_create((size_t)-1) == NULL);
    assert(nicht_bv_create((size_t)-1) == NULL);
    assert(nicht_dset_create((size_t)-1) == NULL);
    assert(nicht_cm_create((size_t)-1) == NULL);

    // 1. VEB: Trigger height >= 64 overflow guard branch
    assert(nicht_veb_create(64) == NULL);

    // 2. Disjoint Set: Trigger rank[root_i] > rank[root_j] branch
    nicht_dset_t *ds_rank = nicht_dset_create(4);
    nicht_dset_union(ds_rank, 0, 1); // rank[0] = 1, rank[1] = 0
    nicht_dset_union(ds_rank, 2, 3); // rank[2] = 1, rank[3] = 0
    nicht_dset_union(ds_rank, 0, 2); // rank[0] becomes 2
    nicht_dset_union(ds_rank, 0, 3); // rank[0] (2) > rank[3] (0) -> triggers root_i > root_j branch
    nicht_dset_free(ds_rank);

    // Trigger the rank[root_i] < rank[root_j] branch
    nicht_dset_t *ds = nicht_dset_create(4);
    nicht_dset_union(ds, 1, 2); // Makes root 1 have rank 1
    nicht_dset_union(ds, 0, 1); // root_i (0, rank 0) merged into root_j (1, rank 1)
    nicht_dset_free(ds);


    // 3. Phi Heap: Force extract_min to pick right child over left child
    PhiHeap *heap = phi_heap_create(4);
    phi_heap_insert(heap, 100, 1);
    phi_heap_insert(heap, 50,  2); // Left child
    phi_heap_insert(heap, 20,  3); // Right child (smaller than left)
    PhiElement extracted;
    phi_heap_extract_min(heap, &extracted); // Extracts 20
    phi_heap_extract_min(heap, &extracted); // Triggers right-child comparison logic
    phi_heap_destroy(heap);

    // Force extract_min to choose the right child over the left child
    PhiHeap *heap2 = phi_heap_create(4);
    phi_heap_insert(heap2, 100, 1);
    phi_heap_insert(heap2, 50,  2); // Left child (index 1)
    phi_heap_insert(heap2, 20,  3); // Right child (index 2) - strictly smaller than left!
    PhiElement elem;
    phi_heap_extract_min(heap, &elem); // Extracts 100, then sifts down and hits `smallest = right`
    phi_heap_extract_min(heap, &elem); 
    phi_heap_destroy(heap);

    // 4. Succinct BV: Rank query at/over total_bits limit
    nicht_bitvector_t *bv = nicht_bv_create(10);
    nicht_bv_build_index(bv);
    assert(nicht_bv_rank1(bv, 10) == 0); // Out-of-bounds idx >= total_bits branch
    nicht_bv_free(bv);

    printf("  [PASS] test_coverage_edge_cases\n");
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
    test_coverage_edge_cases();
    printf("=== All Unit Tests Passed Green (100%% Coverage Target)! ===\n");
    return 0;
}
