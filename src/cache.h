#ifndef __CACHE_H__
#define __CACHE_H__

#include <stdint.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <cassert>
#include "simulated_ints.h"

void prefetchCache(CacheSimulation *cache, uintptr_t address);
bool ownedByUser(CacheSimulation *cache, uintptr_t address);

// We want an 32kB cache with 64B blocks
// 32kB = 2^15B -> 2^15B / 64B = 2^9 blocks
// 2^9 blocks / 8 = 2^6 sets
// 2-way set associative
struct CacheBlock
{
    bool valid;
    uint64_t tag;
    uint8_t data[64];
    // tomato: Maybe some stats that you can keep here
};
struct ReplacementPolicy
{
    // tomato: replacement policy bits will be here
    // you can check them by replacement_policy[set].your_bits
    // next line is a suggestion about how you can store bits for LRU
    uint64_t usage[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    bool tree_lru_bits[7];
};

struct PrefetchRequest
{
    uintptr_t prefetch_addr;
    uint64_t prefetch_time;
};
struct Prefetcher
{
    uintptr_t pointers[4] = {};
    uintptr_t collection[16];
    int collect_counter = 0;
    int collection_index = 0;
    int current_stride_to_manipulate = 0;
    uint64_t strides[4] = {};
    int64_t striding_range_upper[4] = {};
    int64_t striding_range_lower[4] = {};
    CacheSimulation *cache_ptr;
    int condition_function(uintptr_t curr_ptr)
    {
        for (int i = 0; i < 4; i++)
        {
            if (pointers[i] + striding_range_upper[i] > curr_ptr && pointers[i] - striding_range_lower[i] <= curr_ptr)
            {
                if (ownedByUser(cache_ptr, curr_ptr))
                    return i;
            }
        }
        return -1;
    }
    void collect_and_update(uintptr_t curr_ptr)
    {
        // tomato: with this we find finds the state to trigger prefetches on
        // collect some pointers and compare them to see if they are similar
        // is there a striding pattern that we can use here?

        // collect once in n accesses, you can change 15 to change n >= 15 (always in 2^x-1)
        collect_counter = (collect_counter + 1) & 15;
        if (!collect_counter)
        {
            // you can collect this pointer here, manipulate strides and pointers
            collection[collection_index] = curr_ptr;
            collection_index = (collection_index + 1) & 15;
            // tomato: from here figure out if any of your pointers or strides need changing

            // HINT: see if the collections are within some range
            // how common is this access within the range?
            // can you use the range to pick a good stride value?

            // Strides are counted in block sized increments in our case 32kB

            // if a stride is ready to be used you can increment
            // current_stride_to_manipulate = (current_stride_to_manipulate + 1) & 3;
        }
    }
    void prefetch_mems(int selected_stride, uintptr_t address)
    {
        prefetchCache(cache_ptr, address + strides[selected_stride]);
        // tomato: you might want to change your pointer here.
        // you might also want to consider changing where you admit a pointer as a stride candidate.
    }
};
struct int_cached_buffer;
struct CacheSimulation
{
    int hits = 0;
    int misses = 0;
    int reads = 0;
    uint64_t current_time = 0;
    CacheBlock cache[64][8] = {};
    enum Policy
    {
        LRU,
        RANDOM,
        TREELRU
        // tomato: Add your replacement policy here
    };
    Policy policy = LRU;
    ReplacementPolicy replacement_policy[64] = {};
    PrefetchRequest prefetch_queue[32] = {};
    Prefetcher prefetcher;
    bool prefetch_enabled = false;
    // tomato: you could aggregate some stats here.
    int prefetch_index = 0;
    std::vector<int_cached_buffer *> buffers;
};
void initCache();
void registerPolicy(CacheSimulation::Policy policy);
void enablePrefetcher();
void resetCacheStats();
void printStats();
uint64_t cacheAddress(CacheSimulation *cache, uintptr_t address);
int writeCache(CacheSimulation *cache, uintptr_t address, int value);


void initBuffer(int_cached_buffer &buffer, int elem_count);
struct matrix_mult_args
{
    int dim;
    int_cached_buffer src;
    int_cached_buffer src2;
    int_cached_buffer dst;
    int *src_orig;
    int *src2_orig;
    int *dst_result;
};
struct convolution_args
{
    int dim;
    int_cached_buffer src;
    int_cached_buffer ker;
    int_cached_buffer dst;
    int *src_orig;
    int *ker_orig;
    int *dst_result;
};
void initMatrixMultiplicationWithDimensions(int dim, matrix_mult_args &args);
void initConvolutionWithDimensions(int dim, convolution_args &args);
void copyBaseDst(matrix_mult_args &args);
void copyBaseDst(convolution_args &args);
void checkBuffers(matrix_mult_args &args);
void checkBuffers(convolution_args &args);

#endif /* __CACHE_H__ */