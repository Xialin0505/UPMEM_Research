#include <dpu.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "common.h"

#ifndef DPU_BINARY
#define DPU_BINARY "build/hashtable_dpu"
#endif

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_RESET "\x1b[0m"

////static uint8_t test_file[BUFFER_SIZE];

//uint8_t* global_data;
size_t length = SIZE;
dpu_results_t results[NR_DPUS];
int* abortInfo;
struct dpu_set_t dpu_set;
struct dpu_set_t dpu;
HashTable_t* HT;

static uint32_t build_hashtable()
{
    // printf("in build hash table\n");
    // build hashtable
    HT = (HashTable_t*)malloc(sizeof(HashTable_t));
    HT->dummyItem = (struct DataItem*)malloc(sizeof(struct DataItem));
    HT->dummyItem->data = -1;
    HT->dummyItem->key = -1;

    // insert items
    for (int i = 0; i < SIZE; i += 2) {
        insertItem(HT, i, ((i * 2) % SIZE));
    }

    /* ---- create test file ---- */
    // summing up the data in hashtable
    uint32_t sum = 0;

    // iterate through the hashtable
    for (unsigned int i = 0; i < length; i++) {
        struct DataItem* item = searchItem(HT, i);
        // if the item is found, add the data to the sum
        if (item != NULL) {
            sum += item->data;
        }
    }

    return sum;
}

void host_test_func(uint32_t index) {
    // summing up the data in hashtable
    // printf("in host test func XXXXXXXXXXXXXXXXXXX \n");
    uint32_t sum = 0;

    // iterate through the hashtable
    for (unsigned int i = 0; i < length; i++) {
        struct DataItem* item = searchItem(HT, i);
        // if the item is found, add the data to the sum
        if (item != NULL) {
            sum += item->data;
        }
    }
    results[index].tasklet_result[0].hashtablesum = sum;
}

/**
 * @brief Main of the Host Application.
 */
int main()
{
    uint32_t nr_of_dpus;
    uint32_t dpu_hashtablesum;
    uint32_t dpu_cycles;
    bool status = true;

    DPU_ASSERT(dpu_alloc(NR_DPUS, NULL, &dpu_set));
    DPU_ASSERT(dpu_load(dpu_set, DPU_BINARY, NULL));

    DPU_ASSERT(dpu_get_nr_dpus(dpu_set, &nr_of_dpus));
    printf("Allocated %d DPU(s)\n", nr_of_dpus);

    int num_ranks;
    dpu_get_nr_ranks(dpu_set, &num_ranks);
    abortInfo = malloc(num_ranks * sizeof(int));

    uint32_t num_rank = 0;
    dpu_get_nr_ranks(dpu_set, &num_rank);
    printf("Allocated %d rank(s)\n", num_rank);

    // build hashtable
    uint32_t theoretical_hashtablesum = build_hashtable();
    printf("Load input data\n");
    DPU_ASSERT(dpu_copy_to(dpu_set, XSTR(DPU_BUFFER), 0, HT, SIZE));

    printf("Run program on DPU(s)\n");
    DPU_ASSERT(dpu_launch_preempt_restart(dpu_set, DPU_SYNCHRONOUS, &host_test_func, abortInfo));

    DPU_FOREACH (dpu_set, dpu) {
        DPU_ASSERT(dpu_log_read(dpu, stdout));
    }

    printf("Retrieve results\n");
    uint32_t each_dpu;
    DPU_FOREACH (dpu_set, dpu, each_dpu) {
        if (!abortInfo[each_dpu]) {
            DPU_ASSERT(dpu_prepare_xfer(dpu, &results[each_dpu]));
        }
    }
    DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, XSTR(DPU_RESULTS), 0, sizeof(dpu_results_t), DPU_XFER_DEFAULT));

    DPU_FOREACH (dpu_set, dpu, each_dpu) {
        bool dpu_status;
        dpu_hashtablesum = 0;
        dpu_cycles = 0;

        // Retrieve tasklet results and compute the final checksum.
        for (unsigned int each_tasklet = 0; each_tasklet < NR_TASKLETS; each_tasklet++) {
            dpu_result_t *result = &results[each_dpu].tasklet_result[each_tasklet];

            dpu_hashtablesum += result->hashtablesum;
            if (result->cycles > dpu_cycles) {
                dpu_cycles = result->cycles;
            }
        }

        dpu_status = (dpu_hashtablesum == theoretical_hashtablesum);
        status = status && dpu_status;

        printf("DPU execution time  = %g cycles\n", (double)dpu_cycles);
        printf("performance         = %g cycles/byte\n", (double)dpu_cycles / SIZE);
        printf("hashtable_sum computed by the DPU = 0x%08x\n", dpu_hashtablesum);
        printf("actual hashtable_sum value        = 0x%08x\n", theoretical_hashtablesum);
        if (dpu_status) {
            printf("[" ANSI_COLOR_GREEN "OK" ANSI_COLOR_RESET "] hashtable sums are equal\n");
        } else {
            printf("[" ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET "] hashtable sums differ!\n");
        }
    }

    DPU_ASSERT(dpu_free(dpu_set));
    free(abortInfo);
    return status ? 0 : -1;
}



















