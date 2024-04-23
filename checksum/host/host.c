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

#include "common.h"

#ifndef DPU_BINARY
#define DPU_BINARY "build/checksum_dpu"
#endif

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_RESET "\x1b[0m"

static uint8_t test_file[BUFFER_SIZE];

uint8_t* global_data;
size_t length = BUFFER_SIZE;
dpu_results_t results[NR_DPUS];
int* abortInfo;
struct dpu_set_t dpu_set;
struct dpu_set_t dpu;

/**
 * @brief creates a "test file"
 *
 * @return the theorical checksum value
 */
static uint32_t create_test_file()
{
    uint32_t checksum = 0;
    srand(0);

    for (unsigned int i = 0; i < BUFFER_SIZE; i++) {
        test_file[i] = (unsigned char)(rand());
        checksum += test_file[i];
    }
    global_data = test_file;
    return checksum;
}

void host_test_func(uint32_t index) {
    uint32_t checksum = 0;
    srand(0);

    for (unsigned int i = 0; i < length; i++) {
        checksum += global_data[i];
    }
    results[index].tasklet_result[0].checksum = checksum;
}

void host_func_resume() {
    for (int i = 0; i < NR_DPUS; i++) {
        if (!abortInfo[i]) {
            // for (int each_tasklet = 0; each_tasklet < NR_TASKLETS; each_tasklet++) {
            //     dpu_result_t *result = &results[i].tasklet_result[each_tasklet];
            // }
            continue;
        }

        for (int each_tasklet = 0; each_tasklet < NR_TASKLETS; each_tasklet++) {
            dpu_result_t *result = &results[i].tasklet_result[each_tasklet];

            uint32_t checksum = result->checksum;
            uint32_t buffer_idx = result->idx + (NR_TASKLETS * BLOCK_SIZE);

            for (; buffer_idx < BUFFER_SIZE; buffer_idx += (NR_TASKLETS * BLOCK_SIZE)) {

                /* computes the checksum of a cached block */
                for (uint32_t cache_idx = buffer_idx; cache_idx < buffer_idx + BLOCK_SIZE; cache_idx++) {
                    checksum += global_data[cache_idx];
                }
            }
            result->checksum = checksum;
        }
    }
}

/**
 * @brief Main of the Host Application.
 */
int main()
{
    uint32_t nr_of_dpus;
    uint32_t theoretical_checksum, dpu_checksum;
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

    // Create an "input file" with arbitrary data.
    // Compute its theoretical checksum value.
    theoretical_checksum = create_test_file();

    printf("Load input data\n");
    DPU_ASSERT(dpu_copy_to(dpu_set, XSTR(DPU_BUFFER), 0, test_file, BUFFER_SIZE));

    printf("Run program on DPU(s)\n");
    DPU_ASSERT(dpu_launch_preempt(dpu_set, DPU_SYNCHRONOUS, abortInfo));

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

    DPU_FOREACH (dpu_set, dpu, each_dpu) {
        bool dpu_status;
        dpu_checksum = 0;
        dpu_cycles = 0;

        // Retrieve tasklet results and compute the final checksum.
        for (unsigned int each_tasklet = 0; each_tasklet < NR_TASKLETS; each_tasklet++) {
            dpu_result_t *result = &results[each_dpu].tasklet_result[each_tasklet];
            dpu_checksum += result->checksum;
            if (result->cycles > dpu_cycles) {
                dpu_cycles = result->cycles;
            }
        }

        dpu_status = (dpu_checksum == theoretical_checksum);
        status = status && dpu_status;

        printf("DPU execution time  = %g cycles\n", (double)dpu_cycles);
        printf("performance         = %g cycles/byte\n", (double)dpu_cycles / BUFFER_SIZE);
        printf("checksum computed by the DPU = 0x%08x\n", dpu_checksum);
        printf("actual checksum value        = 0x%08x\n", theoretical_checksum);
        if (dpu_status) {
            printf("[" ANSI_COLOR_GREEN "OK" ANSI_COLOR_RESET "] checksums are equal\n");
        } else {
            printf("[" ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET "] checksums differ!\n");
        }

    }

    DPU_ASSERT(dpu_free(dpu_set));
    free(abortInfo);
    return status ? 0 : -1;
}
