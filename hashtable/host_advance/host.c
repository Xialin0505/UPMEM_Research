/*
 * Copyright (c) 2014-2019 - UPMEM
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file host.c
 * @brief Template for a Host Application Source File.
 */

#include <dpu.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>
#include <timer.h>

#include "common.h"

#ifndef DPU_BINARY
#define DPU_BINARY "build/hashtable_dpu"
#endif

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_RESET "\x1b[0m"

#if NR_DPUS > 8
#define THREADS 8
#else
#define THREADS NR_DPUS
#endif

// static hash_t hash_array[TABLE_SIZE];
// static int32_t keys[TABLE_SIZE];
// static int32_t vals[TABLE_SIZE];
// static int8_t  used[TABLE_SIZE];

// int32_t *global_key;
// int32_t *global_val;
// int8_t *global_use;

table_t* global_data;
size_t length = TABLE_SIZE;    // 8 bytes per element
dpu_results_t results[NR_DPUS];
int* abortInfo;
struct dpu_set_t dpu_set;
struct dpu_set_t dpu;
table_t *table;

/**
 * @brief creates a "test file"
 *
 * @return the theorical checksum value
 */
static uint32_t create_test_file()
{
    uint32_t hashsum = 0;

    // initHashTable(&hash_array);

    // printf("create test file\n");

    // insert items
    for (int i = 0; i < TABLE_SIZE; i ++) {
        
        insertItem(table, i, (i % 500));
        // if (i == 20) {
        //     printf("i == 20\n");
        //     printf("hash[0].data = %d\n", ((table->hash_array)[20]).data);
        //     printf("hash[00].data = %d\n", searchItem(table, i));
        // }
    }
    // printf("finish insert\n");
    

    for (unsigned int i = 0; i < TABLE_SIZE; i++) {
        int32_t temp = searchItem(table, i);
        // printf("hash[%d].data = %d\n", i, ((table->hash_array)[i]).data);
        if (temp != -1) {
            hashsum += temp;
        }
        
    }
    // printf("finish search\n");
    global_data = table;
    printf("hashsum in testfile: %d\n", hashsum);
    return hashsum;

    // memset(keys, 0, sizeof(keys));
    // memset(vals, 0, sizeof(vals));
    // memset(used, 0, sizeof(used));

    // for (int i = 0; i < TABLE_SIZE; i++) {
    //     insert(i, (i + 1) % 1000, keys, vals, used);
    // }
    // for (int i = 0; i < TABLE_SIZE; i++) {
    //     int32_t val = search(i, keys, vals, used);
    //     if (val != -1) {
    //         hashsum += val;
    //     }
    // }
    // global_key = keys;
    // global_val = vals;
    // global_use = used;
    // return hashsum;
}

void host_test_func(uint32_t index) {
    uint32_t hashsum = 0;

    for (unsigned int i = 0; i < TABLE_SIZE; i++) {
        hashsum += ((global_data->hash_array)[i]).data;
        // hashsum += global_val[i];
    }
    results[index].tasklet_result[0].hashsum = hashsum;
}

void host_func_resume() {
    // printf("host_func_resume\n");
    #pragma omp parallel for schedule(static) num_threads(THREADS)
    for (int i = 0; i < NR_DPUS; i++) {
        if (!abortInfo[i]) {
            continue;
        }

        for (int each_tasklet = 0; each_tasklet < NR_TASKLETS; each_tasklet++) {
            dpu_result_t *result = &results[i].tasklet_result[each_tasklet];

            uint32_t hashtablesum = result->hashsum;
            uint32_t task_num = TABLE_SIZE / NR_TASKLETS;
            uint32_t buffer_idx = result->idx + 1;
            // printf("resume at %d\n", buffer_idx);
            for (; buffer_idx < (each_tasklet * task_num + task_num); buffer_idx ++) {

                hashtablesum += searchItem(table, buffer_idx);

            }
            result->hashsum = hashtablesum;
        }
    }
}

/**
 * @brief Main of the Host Application.
 */
int main()
{
    uint32_t nr_of_dpus;
    uint32_t theoretical_hashsum, dpu_hashsum;
    uint32_t dpu_cycles;
    table = createTable();
    // for (int i = 0; i < TABLE_SIZE; i++) {
    //     printf("[%d] key: %d, val: %d \n", i, table->hash_array[i].key, table->hash_array[i].data);
    // }
    bool status = true;
    Timer timer;

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

    // Create an "input file" with arbitrary data.
    // Compute its theoretical checksum value.
    theoretical_hashsum = create_test_file();

    start(&timer, 0, 0);

    printf("Load input data\n");

    DPU_FOREACH(dpu_set, dpu)
    {
        DPU_ASSERT(dpu_prepare_xfer(dpu, &(table->hash_array)));
    }
    DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, "hash_array", 0, sizeof(hash_t) * TABLE_SIZE, DPU_XFER_DEFAULT));

    printf("Run program on DPU(s)\n");
    DPU_ASSERT(dpu_launch_preempt_advance(dpu_set, DPU_SYNCHRONOUS, abortInfo));

    DPU_FOREACH (dpu_set, dpu) {
        DPU_ASSERT(dpu_log_read(dpu, stdout));
    }

    printf("Retrieve results\n");
    uint32_t each_dpu;
    DPU_FOREACH (dpu_set, dpu, each_dpu) {
        //if (!abortInfo[each_dpu]) {
            DPU_ASSERT(dpu_prepare_xfer(dpu, &results[each_dpu]));
        //}
    }
    DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, XSTR(DPU_RESULTS), 0, sizeof(dpu_results_t), DPU_XFER_DEFAULT));

    host_func_resume();

    stop(&timer, 0);

    DPU_FOREACH (dpu_set, dpu, each_dpu) {
        bool dpu_status;
        dpu_hashsum = 0;
        dpu_cycles = 0;

        // Retrieve tasklet results and compute the final checksum.
        for (unsigned int each_tasklet = 0; each_tasklet < NR_TASKLETS; each_tasklet++) {
            dpu_result_t *result = &results[each_dpu].tasklet_result[each_tasklet];
            dpu_hashsum += result->hashsum;
            if (result->cycles > dpu_cycles) {
                dpu_cycles = result->cycles;
            }
        }

        dpu_status = (dpu_hashsum == theoretical_hashsum);
        status = status && dpu_status;

        printf("DPU execution time  = %g cycles\n", (double)dpu_cycles);
        printf("performance         = %g cycles/byte\n", (double)dpu_cycles / BUFFER_SIZE);
        printf("hashsum computed by the DPU = 0x%08x\n", dpu_hashsum);
        printf("actual hashsum value        = 0x%08x\n", theoretical_hashsum);
        if (dpu_status) {
            printf("[" ANSI_COLOR_GREEN "OK" ANSI_COLOR_RESET "] hashsums are equal\n");
        } else {
            printf("[" ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET "] hashsums differ!\n");
        }

    }

    printf("End to End Time (ms): ");
	print(&timer, 0, 1);
    printf("\n");

    DPU_ASSERT(dpu_free(dpu_set));
    free(abortInfo);
    return status ? 0 : -1;
}
