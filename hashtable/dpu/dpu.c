#include <defs.h>
#include <mram.h>
#include <perfcounter.h>
#include <stdint.h>
#include <stdio.h>

#include "common.h"

/* Use blocks of 256 bytes */
// #define BLOCK_SIZE (256)

// #define REPEAT_TIMES 10

// __dma_aligned uint8_t DPU_CACHES[NR_TASKLETS][BLOCK_SIZE];
__host dpu_results_t DPU_RESULTS;
__host int finish_regular_work;
__host int start_stealing;

// __mram_noinit uint8_t DPU_BUFFER[BUFFER_SIZE];
__mram_noinit hash_t hash_array[TABLE_SIZE];


/**
 * @fn main
 * @brief main function executed by each tasklet
 * @return the hashtable sum result
 */
int main()
{
    uint32_t tasklet_id = me();
    dpu_result_t *result = &DPU_RESULTS.tasklet_result[tasklet_id];
    int32_t hashtablesum = 0;
    result->hashsum = 0;

    /* Initialize once the cycle counter */
    if (tasklet_id == 0)
        perfcounter_config(COUNT_CYCLES, true);

    uint32_t task_num = TABLE_SIZE / NR_TASKLETS;
    uint32_t buffer_idx = tasklet_id * task_num;

    for (; buffer_idx < (tasklet_id * task_num + task_num); buffer_idx ++) {
        /* search hash table algo */
        int hashIndex = hashCode(buffer_idx);
        int it = 0;
        for (it = 0; it < TABLE_SIZE; it++, hashIndex++) {
            if (hash_array[hashIndex].key == buffer_idx) {
                hashtablesum += hash_array[hashIndex].data;
                break;
            }
            else if (hash_array[hashIndex].key == -1) {
                break;
            }
            hashIndex %= TABLE_SIZE;
        }

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


