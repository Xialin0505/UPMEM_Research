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
#include <x86intrin.h>

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
    bool status = true;
    Timer timer;

    

    start(&timer, 0, 0);
	
	uint64_t startc = _rdtsc();
	theoretical_hashsum = create_test_file();
	uint64_t endc = _rdtsc();

	double cycles = (double)(endc - startc);
	printf("CPU version CPU cycles : %g\n", cycles);
    stop(&timer, 0);

    

    printf("End to End Time (ms): ");
	print(&timer, 0, 1);
    printf("\n");

   
    return status ? 0 : -1;
}
