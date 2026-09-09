#ifndef NICHT_SUCCINCT_BV_H
#define NICHT_SUCCINCT_BV_H

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>

typedef struct {
    uint64_t *words;      // Raw bit vector data
    uint32_t *block_rank; // Interleaved cumulative rank counts
    size_t total_bits;
    size_t num_words;
    size_t num_blocks;
} nicht_bitvector_t;

static inline nicht_bitvector_t* nicht_bv_create(size_t total_bits) {
    // Guard against arithmetic overflow (SIZE_MAX - 63)
    if (total_bits > SIZE_MAX - 63) return NULL;
    
    nicht_bitvector_t *bv = (nicht_bitvector_t*)malloc(sizeof(nicht_bitvector_t));
    bv->total_bits = total_bits;
    bv->num_words = (total_bits + 63) / 64;
    bv->num_blocks = (bv->num_words + 7) / 8; // 512-bit block intervals

    // 64-byte cache-line alignment
    if(posix_memalign((void**)&bv->words, 64, bv->num_words * sizeof(uint64_t)) != 0) {
        free(bv);
        return NULL;
    }
    if(posix_memalign((void**)&bv->block_rank, 64, bv->num_blocks * sizeof(uint32_t)) != 0) {
        free(bv->words);
        free(bv);
        return NULL;
    }

    for (size_t i = 0; i < bv->num_words; i++) bv->words[i] = 0;
    for (size_t i = 0; i < bv->num_blocks; i++) bv->block_rank[i] = 0;

    return bv;
}

static inline void nicht_bv_set(nicht_bitvector_t *bv, size_t bit_idx) {
    bv->words[bit_idx / 64] |= (1ULL << (bit_idx % 64));
}

// Build rank index table (call after setting bits)
static inline void nicht_bv_build_index(nicht_bitvector_t *bv) {
    uint32_t running_rank = 0;
    for (size_t i = 0; i < bv->num_words; i++) {
        if (i % 8 == 0) {
            bv->block_rank[i / 8] = running_rank;
        }
        running_rank += __builtin_popcountll(bv->words[i]);
    }
}

// O(1) Rank Query: count 1-bits up to idx (inclusive)
static inline size_t nicht_bv_rank1(const nicht_bitvector_t *bv, size_t idx) {
    if (idx >= bv->total_bits) idx = bv->total_bits - 1;
    
    size_t word_idx = idx / 64;
    size_t block_idx = word_idx / 8;
    
    size_t rank = bv->block_rank[block_idx];
    for (size_t i = block_idx * 8; i < word_idx; i++) {
        rank += __builtin_popcountll(bv->words[i]);
    }
    
    // Safely generate bitmask for remainder bits within word [0 .. idx % 64]
    uint32_t bit_pos = idx % 64;
    uint64_t mask = (bit_pos == 63) ? ~0ULL : ((1ULL << (bit_pos + 1)) - 1);
    
    rank += __builtin_popcountll(bv->words[word_idx] & mask);
    
    return rank;
}

static inline void nicht_bv_free(nicht_bitvector_t *bv) {
    free(bv->words);
    free(bv->block_rank);
    free(bv);
}

#endif
