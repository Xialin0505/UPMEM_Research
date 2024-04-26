#ifndef KECCAK_DPU_PARAMS_H
#define KECCAK_DPU_PARAMS_H

#include <stdint.h>

struct dpu_params {
    uint32_t fkey; /* first key */
    uint32_t lkey; /* last key */
    uint32_t loops; /* #loops */
};

typedef struct {
    uint64_t sum;
    uint64_t cycles;
	uint32_t cur_key;
	uint32_t loops;
	uint32_t lkey;
	uint32_t cur_loop;
	uint64_t tst[25];
} dpu_result_t;

typedef struct {
    dpu_result_t tasklet_result[NR_TASKLETS];
} dpu_results_t;

#endif /* KECCAK_DPU_PARAMS_H */
