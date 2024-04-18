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

__mram_noinit uint8_t DPU_BUFFER[SIZE];

int isSame(const uint32_t a[], int n)
{   
    while(--n>0 && a[n]==a[0]);
    return n!=0;
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
    uint32_t hashtablesum = 0;
    result->hashtablesum = 0;

    uint32_t sum_arr[REPEAT_TIMES] = {0};

    /* Initialize once the cycle counter */
    if (tasklet_id == 0)
        perfcounter_config(COUNT_CYCLES, true);

    /* xxxxxxxxxxxxxxx increase compute intensity by repeat checksum n times xxxxxxxxxxxxxxxxxxxxxxx*/
    for (uint32_t i = 0; i < REPEAT_TIMES; i++) {
        for (uint32_t buffer_idx = tasklet_id * BLOCK_SIZE; buffer_idx < SIZE; buffer_idx += (NR_TASKLETS * BLOCK_SIZE)) {

            /* load cache with current mram block. */
            mram_read(&DPU_BUFFER[buffer_idx], cache, BLOCK_SIZE);

            /* computes the checksum of a cached block */
            for (uint32_t cache_idx = 0; cache_idx < BLOCK_SIZE; cache_idx++) {
                hashtablesum += cache[cache_idx];
            }
        }
        sum_arr[i] = hashtablesum;
        hashtablesum = 0;
    }

    finish_regular_work = 1;

    if (isSame(sum_arr, REPEAT_TIMES)) { // check if same solution each time
        hashtablesum = sum_arr[0];
    }
    else {
        hashtablesum = sum_arr[REPEAT_TIMES - 1]; 
    }

    /* xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx */

    /* keep the 32-bit LSB on the 64-bit cycle counter */
    result->cycles = (uint32_t)perfcounter_get();
    result->hashtablesum = hashtablesum;

    printf("[%02d] Checksum = 0x%08x\n", tasklet_id, result->hashtablesum);
    return 0;
}


