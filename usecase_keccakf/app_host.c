/*
 * Copyright 2018 - UPMEM
 *
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)•
 * any later version.
 *
 */

#include "keccakf_dpu_params.h"
#include <dpu.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include "timer.h"
#include <x86intrin.h>

typedef uint64_t u64;

#define KECCAK_ROUNDS 24

#define ROTL64(x, y) (((x) << (y)) | ((x) >> (64 - (y))))

static const u64 keccakf_rndc[24] = { 0x0000000000000001ULL, 0x0000000000008082ULL, 0x800000000000808aULL, 0x8000000080008000ULL,
    0x000000000000808bULL, 0x0000000080000001ULL, 0x8000000080008081ULL, 0x8000000000008009ULL, 0x000000000000008aULL,
    0x0000000000000088ULL, 0x0000000080008009ULL, 0x000000008000000aULL, 0x000000008000808bULL, 0x800000000000008bULL,
    0x8000000000008089ULL, 0x8000000000008003ULL, 0x8000000000008002ULL, 0x8000000000000080ULL, 0x000000000000800aULL,
    0x800000008000000aULL, 0x8000000080008081ULL, 0x8000000000008080ULL, 0x0000000080000001ULL, 0x8000000080008008ULL };

static void keccakf(u64 st[25])
{
    int round;
    u64 t, t1, t2, bc0, bc1, bc2, bc3, bc4;

    u64 st0 = st[0];
    u64 st1 = st[1];
    u64 st2 = st[2];
    u64 st3 = st[3];
    u64 st4 = st[4];
    u64 st5 = st[5];
    u64 st6 = st[6];
    u64 st7 = st[7];
    u64 st8 = st[8];
    u64 st9 = st[9];
    u64 st10 = st[10];
    u64 st11 = st[11];
    u64 st12 = st[12];
    u64 st13 = st[13];
    u64 st14 = st[14];
    u64 st15 = st[15];
    u64 st16 = st[16];
    u64 st17 = st[17];
    u64 st18 = st[18];
    u64 st19 = st[19];
    u64 st20 = st[20];
    u64 st21 = st[21];
    u64 st22 = st[22];
    u64 st23 = st[23];
    u64 st24 = st[24];
    for (round = 0; round < KECCAK_ROUNDS; round++) {

        bc0 = st0 ^ st5 ^ st10 ^ st15 ^ st20;
        bc1 = st1 ^ st6 ^ st11 ^ st16 ^ st21;
        bc2 = st2 ^ st7 ^ st12 ^ st17 ^ st22;
        bc3 = st3 ^ st8 ^ st13 ^ st18 ^ st23;
        bc4 = st4 ^ st9 ^ st14 ^ st19 ^ st24;
        t = bc4 ^ ((bc1 << 1) | (bc1 >> 63));
        st0 ^= t;
        st5 ^= t;
        st10 ^= t;
        st15 ^= t;
        st20 ^= t;
        t = bc0 ^ ((bc2 << 1) | (bc2 >> 63));
        st1 ^= t;
        st6 ^= t;
        st11 ^= t;
        st16 ^= t;
        st21 ^= t;
        t = bc1 ^ ((bc3 << 1) | (bc3 >> 63));
        st2 ^= t;
        st7 ^= t;
        st12 ^= t;
        st17 ^= t;
        st22 ^= t;
        t = bc2 ^ ((bc4 << 1) | (bc4 >> 63));
        st3 ^= t;
        st8 ^= t;
        st13 ^= t;
        st18 ^= t;
        st23 ^= t;
        t = bc3 ^ ((bc0 << 1) | (bc0 >> 63));
        st4 ^= t;
        st9 ^= t;
        st14 ^= t;
        st19 ^= t;
        st24 ^= t;

        t1 = st1;
        t2 = st10;
        st10 = ROTL64(t1, 1);
        t1 = st7;
        st7 = ROTL64(t2, 3);
        t2 = st11;
        st11 = ROTL64(t1, 6);
        t1 = st17;
        st17 = ROTL64(t2, 10);
        t2 = st18;
        st18 = ROTL64(t1, 15);
        t1 = st3;
        st3 = ROTL64(t2, 21);
        t2 = st5;
        st5 = ROTL64(t1, 28);
        t1 = st16;
        st16 = ROTL64(t2, 36);
        t2 = st8;
        st8 = ROTL64(t1, 45);
        t1 = st21;
        st21 = ROTL64(t2, 55);
        t2 = st24;
        st24 = ROTL64(t1, 2);
        t1 = st4;
        st4 = ROTL64(t2, 14);
        t2 = st15;
        st15 = ROTL64(t1, 27);
        t1 = st23;
        st23 = ROTL64(t2, 41);
        t2 = st19;
        st19 = ROTL64(t1, 56);
        t1 = st13;
        st13 = ROTL64(t2, 8);
        t2 = st12;
        st12 = ROTL64(t1, 25);
        t1 = st2;
        st2 = ROTL64(t2, 43);
        t2 = st20;
        st20 = ROTL64(t1, 62);
        t1 = st14;
        st14 = ROTL64(t2, 18);
        t2 = st22;
        st22 = ROTL64(t1, 39);
        t1 = st9;
        st9 = ROTL64(t2, 61);
        t2 = st6;
        st6 = ROTL64(t1, 20);
        t1 = st1;
        st1 = ROTL64(t2, 44);

        bc0 = st0;
        bc1 = st1;
        bc2 = st2;
        bc3 = st3;
        bc4 = st4;
        st0 ^= ~bc1 & bc2;
        st1 ^= ~bc2 & bc3;
        st2 ^= ~bc3 & bc4;
        st3 ^= ~bc4 & bc0;
        st4 ^= ~bc0 & bc1;
        bc0 = st5;
        bc1 = st6;
        bc2 = st7;
        bc3 = st8;
        bc4 = st9;
        st5 ^= ~bc1 & bc2;
        st6 ^= ~bc2 & bc3;
        st7 ^= ~bc3 & bc4;
        st8 ^= ~bc4 & bc0;
        st9 ^= ~bc0 & bc1;
        bc0 = st10;
        bc1 = st11;
        bc2 = st12;
        bc3 = st13;
        bc4 = st14;
        st10 ^= ~bc1 & bc2;
        st11 ^= ~bc2 & bc3;
        st12 ^= ~bc3 & bc4;
        st13 ^= ~bc4 & bc0;
        st14 ^= ~bc0 & bc1;
        bc0 = st15;
        bc1 = st16;
        bc2 = st17;
        bc3 = st18;
        bc4 = st19;
        st15 ^= ~bc1 & bc2;
        st16 ^= ~bc2 & bc3;
        st17 ^= ~bc3 & bc4;
        st18 ^= ~bc4 & bc0;
        st19 ^= ~bc0 & bc1;
        bc0 = st20;
        bc1 = st21;
        bc2 = st22;
        bc3 = st23;
        bc4 = st24;
        st20 ^= ~bc1 & bc2;
        st21 ^= ~bc2 & bc3;
        st22 ^= ~bc3 & bc4;
        st23 ^= ~bc4 & bc0;
        st24 ^= ~bc0 & bc1;

        st0 ^= keccakf_rndc[round];
    }
    st[0] = st0;
    st[1] = st1;
    st[2] = st2;
    st[3] = st3;
    st[4] = st4;
    st[5] = st5;
    st[6] = st6;
    st[7] = st7;
    st[8] = st8;
    st[9] = st9;
    st[10] = st10;
    st[11] = st11;
    st[12] = st12;
    st[13] = st13;
    st[14] = st14;
    st[15] = st15;
    st[16] = st16;
    st[17] = st17;
    st[18] = st18;
    st[19] = st19;
    st[20] = st20;
    st[21] = st21;
    st[22] = st22;
    st[23] = st23;
    st[24] = st24;
}

double dml_micros()
{
    static struct timeval tv;
    gettimeofday(&tv, 0);
    return ((tv.tv_sec * 1000000.0) + tv.tv_usec);
}

int* abortInfo;
uint32_t nr_of_dpus, nr_of_tasklets;

dpu_results_t *total_results;
uint64_t total_sum = 0;


void host_resume(){
    printf("host_resume\n");
    int thread = 8;
    if (nr_of_dpus < 8) {
        thread = nr_of_dpus;
    }

	#pragma omp parallel for schedule(static) num_threads(thread)
	for(int i = 0;i < nr_of_dpus;i++){
		if(!abortInfo[i]){
			continue;
		}
		for(int t = 0; t < NR_TASKLETS;t++){
			
			uint32_t key = total_results[i].tasklet_result[t].cur_key;
			uint32_t lkey = total_results[i].tasklet_result[t].lkey;
			uint32_t loops = total_results[i].tasklet_result[t].loops;
			uint32_t cur_loop = total_results[i].tasklet_result[t].cur_loop;
			
			for (; key < lkey; key++) {
				uint64_t st[25];
				for (int j = 0; j < 25; j++)
	            {
		           st[j] = total_results[i].tasklet_result[t].tst[j];
	            }

                for (int l = cur_loop; l < loops; l++) {
                  keccakf(st);
                  uint64_t tmp = st[0];
                  for (int i = 0; i < 24; i++)
                    st[i] = st[i + 1];
                  st[24] = tmp; // rotate st
                }

                for (int i = 0; i < 25; i++)
                  total_sum ^= st[i];
		
            }
		}
	}


}

int main(int argc, char **argv)
{
    const uint64_t fkey = argc > 1 ? atoi(argv[1]) : 0; /* first key */
    const uint64_t lkey = argc > 2 ? atoi(argv[2]) : 1 << 10; /* last  key */
    const uint64_t loops = argc > 3 ? atoi(argv[3]) : 1 << 20; /* #loops    */
    const uint64_t nkey = lkey - fkey;
    struct dpu_set_t dpus;
    struct dpu_set_t dpu;
    struct dpu_program_t *dpu_program;
    unsigned int t, i, idx;
    int res = -1;

    Timer timer;

    DPU_ASSERT(dpu_alloc(DPU_ALLOCATE_ALL, NULL, &dpus));
    DPU_ASSERT(dpu_load(dpus, DPU_BINARY, &dpu_program));

    DPU_ASSERT(dpu_get_nr_dpus(dpus, &nr_of_dpus));
    nr_of_tasklets = nr_of_dpus * NR_TASKLETS;
	total_results = calloc(sizeof(total_results[0]), nr_of_dpus);
    printf("Allocated %d DPU(s) x %d Threads\n", nr_of_dpus, NR_TASKLETS);

    int rank_num;
	dpu_get_nr_ranks(dpus, &rank_num);
	abortInfo = malloc(rank_num * sizeof(int));

    start(&timer, 0, 0);

    /* send parameters to the DPU tasklets */
    idx = 0;
    DPU_FOREACH (dpus, dpu) {
        struct dpu_symbol_t tasklet_params;
        DPU_ASSERT(dpu_get_symbol(dpu_program, "tasklet_params", &tasklet_params));
        struct dpu_params params;
        params.loops = loops;
        for (t = 0; t < NR_TASKLETS; t++, idx++) {
            params.fkey = fkey + (nkey * idx + nr_of_tasklets - 1) / nr_of_tasklets;
            params.lkey = fkey + (nkey * (idx + 1) + nr_of_tasklets - 1) / nr_of_tasklets;
            if (params.lkey > lkey)
                params.lkey = lkey;
            printf(" Thread %02d-%02d %d->%d\n", idx / NR_TASKLETS, t, params.fkey, params.lkey);
            DPU_ASSERT(dpu_copy_to_symbol(dpu, tasklet_params, t * sizeof(params), (const uint8_t *)&params, sizeof(params)));
        }
    }

    printf("Run program on DPU(s)\n");

	double micros = dml_micros();
    DPU_ASSERT(dpu_launch_preempt(dpus, DPU_SYNCHRONOUS, abortInfo));
	micros -= dml_micros();

    printf("Retrieve results\n");
    dpu_result_t *results;
    unsigned long long sum = 0;
    uint64_t cycles = 0;
    results = calloc(sizeof(results[0]), nr_of_dpus);
    if (!results)
        goto err;
    i = 0;
    DPU_FOREACH (dpus, dpu) {
        /* Retrieve tasklet results and compute the final keccak. */
        struct dpu_symbol_t tasklet_results;
        DPU_ASSERT(dpu_get_symbol(dpu_program, "tasklet_results", &tasklet_results));
        for (t = 0; t < NR_TASKLETS; t++) {
            dpu_result_t tasklet_result = { 0 };
            DPU_ASSERT(dpu_copy_from_symbol(
                dpu, tasklet_results, t * sizeof(tasklet_result), (uint8_t *)&tasklet_result, sizeof(tasklet_result)));
			total_results[i].tasklet_result[t].cur_key = tasklet_result.cur_key;
			total_results[i].tasklet_result[t].lkey = tasklet_result.lkey;
			total_results[i].tasklet_result[t].cur_loop = tasklet_result.cur_loop;
			printf("cur_key:%d ;lkey:%d; cur_loop: %d, cur_sum:%d\n",tasklet_result.cur_key, tasklet_result.lkey, tasklet_result.cur_loop, tasklet_result.sum);
			total_results[i].tasklet_result[t].loops = tasklet_result.loops;
			for (int j = 0; j < 25; j++)
	        {
		      total_results[i].tasklet_result[t].tst[j] = tasklet_result.tst[j];
	        }
            results[i].sum ^= tasklet_result.sum;
            if (tasklet_result.cycles > results[i].cycles)
                results[i].cycles = tasklet_result.cycles;
        }
        sum ^= results[i].sum;
        if (results[i].cycles > cycles)
            cycles = results[i].cycles;
        i++;
    }

	uint64_t startc = _rdtsc();
	host_resume();
	uint64_t endc = _rdtsc();

	double ccycles = (double)(endc - startc);
	printf("Host resume CPU cycles : %g\n", ccycles);
	
    sum ^= total_sum;

    stop(&timer, 0);

    //for (i = 0; i < nr_of_dpus; i++)
        //printf("DPU cycle count = %" PRIu64 " cc\n", results[i].cycles);


    double secs = -micros / 1000000.0;
    double Mks = loops * (lkey - fkey) / 1000000.0 / secs * 2048;
    printf("_F_ fkey= %6u lkey= %6u loops= %6d SUM= %llx seconds= %1.6lf   Mks= %1.6lf\n", (unsigned int)fkey, (unsigned int)lkey,
        (unsigned int)loops, sum, secs, Mks);

    double dpu_secs = cycles / 600000000.0;
    double dpu_Mks = loops * (lkey - fkey) / 1000000.0 / dpu_secs / nr_of_dpus;
    printf("DPU fkey= %6u lkey= %6u loops= %6d SUM= %llx seconds= %1.6lf   Mks= %1.6lf\n", (unsigned int)fkey, (unsigned int)lkey,
        (unsigned int)loops, sum, dpu_secs, dpu_Mks);

    
    free(results);
    free(abortInfo);
	free(total_results);
    res = 0;

    printf("End to End Time (ms): ");
	print(&timer, 0, 1);
    printf("\n");

err:
    DPU_ASSERT(dpu_free(dpus));
    return res;
}
