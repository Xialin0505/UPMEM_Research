#include <defs.h>
#include <mram.h>
#include <perfcounter.h>
#include <stdint.h>
#include <stdio.h>

#include "common.h"

/* Use blocks of 256 bytes */
#define BLOCK_SIZE (256)

#define REPEAT_TIMES 10

__dma_aligned uint8_t DPU_CACHES[NR_TASKLETS][BLOCK_SIZE];
__host dpu_results_t DPU_RESULTS;
__host int finish_regular_work;
__host int start_stealing;

// __mram_noinit uint8_t DPU_BUFFER[BUFFER_SIZE];
__mram_noinit hash_t hash_array[TABLE_SIZE];


// uint32_t searchNsum(uint8_t *cache) {
//     uint32_t sum = 0;
    

//     // for (uint32_t i = 0; i < (BLOCK_SIZE / 4); i += 2) {
//     //     uint32_t val = 0;
//     //     val |= (uint32_t)cache[4 * i] << 24;
//     //     val |= (uint32_t)cache[4 * i + 1] << 16;
//     //     val |= (uint32_t)cache[4 * i + 2] << 8;
//     //     val |= (uint32_t)cache[4 * i + 3];

//     //     sum += val;
//     // }

//     for (uint32_t cache_idx = 0; cache_idx < BLOCK_SIZE; cache_idx += (sizeof(uint32_t)*2)) {
//         uint32_t val = 0;
//         memcpy(&val, &cache[cache_idx], sizeof(uint32_t));
//         sum += val;
//     }
//     return sum;

// }

// // Search function
// int32_t searchItem(int32_t key, const int32_t *keys, int32_t *values, const int8_t *used) {
//     int index = hash(key);
//     while (*(used + index)) {
//         if (*(keys + index) == key) {
//             return *(values + index);  // Return the corresponding value
//         }
//         index = (index + 1) % TABLE_SIZE;  // Linear probing
//     }
//     return -1;  // Key not found
// }

int32_t search(hash_t arr[], int key)
{
    // get the hash
    int hashIndex = hashCode(key);
    int it = 0;
    // printf("search item\n");
    // move in array until an empty
    // while (arr[hashIndex].key != -1)
    // {

    //     if (arr[hashIndex].key == key)
    //         return arr[hashIndex].data;

    //     if (it == TABLE_SIZE) {
    //         break;
    //     }
    //     // go to next cell
    //     ++hashIndex;
    //     it++;

    //     // wrap around the table
    //     hashIndex %= TABLE_SIZE;
    // }

    for (it = 0; it < TABLE_SIZE; it++, hashIndex++) {
        if (arr[hashIndex].key == key) {
            printf("return data %d\n", arr[hashIndex].data);
            return arr[hashIndex].data;
        }
        else if (arr[hashIndex].key == -1) {
            break;
        }
        hashIndex %= TABLE_SIZE;
    }

    return -1;
}

/**
 * @fn main
 * @brief main function executed by each tasklet
 * @return the hashtable sum result
 */
int main()
{
    uint32_t tasklet_id = me();
    uint8_t *cache = DPU_CACHES[tasklet_id];
    dpu_result_t *result = &DPU_RESULTS.tasklet_result[tasklet_id];
    int32_t hashtablesum = 0;
    result->hashsum = 0;

    // int32_t *keys = (int32_t*) mem_alloc(BLOCK_SIZE);
    // int32_t *vals = (int32_t*) mem_alloc(BLOCK_SIZE);
    // int32_t *used = (int32_t*) mem_alloc(BLOCK_SIZE);

    /* Initialize once the cycle counter */
    if (tasklet_id == 0)
        perfcounter_config(COUNT_CYCLES, true);

    uint32_t task_num = TABLE_SIZE / NR_TASKLETS;
    uint32_t buffer_idx = tasklet_id * task_num;
    // hash_t *arr = (hash_t*)hash_array;
    // printf("hash[0].data = %d\n", (hash_t*)(hash_array)[30].data);
    for (; buffer_idx < (tasklet_id * task_num + task_num); buffer_idx ++) {

        /* load cache with current mram block. */
        // mram_read(&DPU_BUFFER[buffer_idx], cache, BLOCK_SIZE);

        // call search, return hash sum
        // hashtablesum += searchNsum(cache);

        // hashtablesum += searchItem(buffer_idx, (int32_t*)keys, (int32_t*)vals, (int8_t*)used);
        /* search hash table algo */
        int hashIndex = hashCode(buffer_idx);
        // printf("hashidx = %d\n", hashIndex);
        int it = 0;
        int32_t temp = 0;
        for (it = 0; it < TABLE_SIZE; it++, hashIndex++) {
            if (hash_array[hashIndex].key == buffer_idx) {
                // printf("return data %d\n", hash_array[hashIndex].data);
                temp = hash_array[hashIndex].data;
                break;
            }
            else if (hash_array[hashIndex].key == -1) {
                break;
            }
            hashIndex %= TABLE_SIZE;
        }
        // int32_t temp = search(hash_array, buffer_idx);
        
        // printf("temp = %d \n", temp);
        
        /* end search algo */
        hashtablesum += temp;


        /* computes the checksum of a cached block */
        // for (uint32_t cache_idx = 0; cache_idx < BLOCK_SIZE; cache_idx++) {
        //     hashtablesum += cache[cache_idx];
        // }
        result->hashsum = hashtablesum;
        result->idx = buffer_idx;
        result->cycles = (uint32_t)perfcounter_get();
    }
    finish_regular_work = 1;

    /* keep the 32-bit LSB on the 64-bit cycle counter */
    result->cycles = (uint32_t)perfcounter_get();
    result->hashsum = hashtablesum;
    result->idx = buffer_idx;

    printf("[%02d] hashsum = 0x%08x\n", tasklet_id, result->hashsum);
    return 0;
}


