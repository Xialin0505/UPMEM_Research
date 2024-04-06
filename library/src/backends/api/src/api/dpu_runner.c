/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "dpu.h"
#include <dpu_rank.h>
#include <dpu_management.h>
#include <dpu_polling.h>
#include <dpu_api_verbose.h>
#include <dpu_types.h>
#include <dpu_error.h>
#include <dpu_attributes.h>
#include <dpu_log_utils.h>
#include <dpu_thread_job.h>
#include <dpu_mask.h>
#include <dpu_config.h>

#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#define RANK_THRESHOLD 0.4
#define EXECUTION_WINDOW 0.02

#define STOPci(cc, pc)                                                                                                           \
    ((dpuinstruction_t)(0x7ef320000000l | (((((dpuinstruction_t)cc) >> 0) & 0xf) << 24)                                          \
        | (((((dpuinstruction_t)pc) >> 0) & 0xffff) << 0)))

#define BOOT_CC_TRUE (1)

struct preempt_info{
    pthread_t preempt_thread;
    struct dpu_set_t* dpu_set;

    long long avg_execution_dur;
    int nr_rank;
    int nr_rank_threshold;
    short* rank_done;
    long long check_interval;
    long long* execution_times;
    struct timeval startTime;
    int num_done;

    uint8_t* input;
    size_t data_length;
    uint32_t* output;
    void (*host_func)(uint8_t*, uint32_t*, size_t);
};

static struct preempt_info preempt;

static const char *
dpu_launch_policy_to_string(dpu_launch_policy_t policy)
{
    switch (policy) {
        case DPU_ASYNCHRONOUS:
            return "ASYNCHRONOUS";
        case DPU_SYNCHRONOUS:
            return "SYNCHRONOUS";
        default:
            return "UNKNOWN";
    }
}

static void nano_sleep(long duration) {
    nanosleep((const struct timespec[]){{0, duration}}, NULL);
} 

static long long getElaspedTime() {
    struct timeval nowTime;
    gettimeofday(&nowTime, NULL);

    double elapsedTime;
    elapsedTime = (nowTime.tv_sec - preempt.startTime.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (nowTime.tv_usec - preempt.startTime.tv_usec) / 1000.0;   // us to ms

    return (long long)elapsedTime;
}

/*
output the 
*/
static bool determine_struggle_dpu() {
    if (preempt.num_done < preempt.nr_rank_threshold) return false;
    
    long long elapsed = getElaspedTime();
    if (elapsed * (1 + EXECUTION_WINDOW) > preempt.avg_execution_dur) {
        return true;
    }

    return false;
}  

static void each_rank_status() {
    for (uint32_t each_rank = 0; each_rank < preempt.nr_rank; ++each_rank) {
        dpu_error_t status;
        bool rank_done;
        bool rank_fault;

        if ((status = dpu_status_rank(preempt.dpu_set->list.ranks[each_rank], &rank_done, &rank_fault)) != DPU_OK) {
            preempt.dpu_set->list.ranks[each_rank]->api.abort = true;
        }

        if (rank_done) {
            if (preempt.rank_done[each_rank] == 0){
                long long elapsedTime = getElaspedTime();
                preempt.rank_done[each_rank] = 1;
                preempt.execution_times[each_rank] = elapsedTime;

                preempt.avg_execution_dur = preempt.avg_execution_dur * preempt.num_done + elapsedTime;
                preempt.num_done ++;
                preempt.avg_execution_dur /= preempt.num_done;

                preempt.dpu_set->list.ranks[each_rank]->api.abort = false;
            }
        } else if (rank_fault) {
            preempt.rank_done[each_rank] = 2;
        } 
    }
}

static void print_rank_status() {
    for (int i = 0; i < preempt.nr_rank; i++) {
        printf("done %d ", preempt.rank_done[i]);
    }
    printf("\n");

    for (int i = 0; i < preempt.nr_rank; i++) {
        if (preempt.rank_done[i]) {
            printf("execution time %lld ", preempt.execution_times[i]);
        } else {
            printf("not done ");
        }
    }
    printf("\n");
    printf("avg execution time %lld\n", preempt.avg_execution_dur);
}

void abort_struggling_dpu(int rankIdx) {
    printf("abort_struggling_dpu %d\n", rankIdx);
    preempt.dpu_set->list.ranks[rankIdx]->api.thread_info.should_stop = true;
    preempt.dpu_set->list.ranks[rankIdx]->api.abort = true;
    if (preempt.host_func != NULL) {
        preempt.host_func(preempt.input, &(preempt.output[rankIdx]), preempt.data_length);
    }
    dpu_reset_rank(preempt.dpu_set->list.ranks[rankIdx]);
}

void abort_struggling_dpus() {
    printf("abort_struggling_dpus\n"); 
    for (int i = 0; i < preempt.nr_rank; i++) {
        if (!preempt.rank_done[i]) {
            abort_struggling_dpu(i);
        }
    }
}

void * dpu_poll_status(void * arg) {
    struct dpu_thread_job_sync * sync = (struct dpu_thread_job_sync*) arg;
    struct timeval nowTime;

    gettimeofday(&preempt.startTime, NULL);
    bool abort = false;

    while (!sync->stop_polling) {
        int running_rank_num = sync -> nr_ranks;
        nano_sleep(preempt.check_interval);

        each_rank_status();
        if (determine_struggle_dpu()) {
            abort = true;
            sync->stop_polling = true;
        }
    }

    if (abort) {
        abort_struggling_dpus();
    }

    return NULL;
}

static void init_preempt(struct dpu_set_t * dpu_set, struct dpu_thread_job_sync * sync,
                            void (*host_func)(uint8_t*, uint32_t*, size_t),
                            uint8_t* input, uint32_t* output, size_t length) {
    preempt.nr_rank = sync->nr_ranks;
    preempt.check_interval = 50000000L;
    preempt.dpu_set = dpu_set;
    preempt.num_done = 0;
    preempt.avg_execution_dur = 0;
    preempt.nr_rank_threshold = RANK_THRESHOLD * preempt.nr_rank;
    preempt.host_func = host_func;
    preempt.input = input;
    preempt.data_length = length;
    preempt.output = output;

    preempt.execution_times = malloc(sizeof(long long) * preempt.nr_rank);
    memset(preempt.execution_times, 0, sizeof(long long) * preempt.nr_rank);
    preempt.rank_done = malloc(sizeof(short) * preempt.nr_rank);
    memset(preempt.rank_done, 0, sizeof(short) * preempt.nr_rank);

    sync->stop_polling = false;
    int ret = pthread_create(&preempt.preempt_thread, NULL, dpu_poll_status, (void*)sync);
}

static void clean_preempt(struct dpu_thread_job_sync * sync) {
    sync->stop_polling = true;
    pthread_join(preempt.preempt_thread, NULL);
    free(preempt.execution_times);
    free(preempt.rank_done);
}

__API_SYMBOL__ dpu_error_t
dpu_launch_preempt_restart(struct dpu_set_t dpu_set, dpu_launch_policy_t policy,
                    void (*host_func)(uint8_t*, uint32_t*, size_t), 
                    uint8_t* input, uint32_t* output, size_t length)
{
    dpu_error_t status = DPU_OK;
    LOG_FN(DEBUG, "%s", dpu_launch_policy_to_string(policy));
    printf("dpu_launch_preempt_restart\n");

    if (dpu_set.kind != DPU_SET_RANKS && dpu_set.kind != DPU_SET_DPU) {
        return DPU_ERR_INTERNAL;
    }

    uint32_t nr_ranks;
    struct dpu_rank_t **ranks;
    struct dpu_rank_t *rank;
    switch (dpu_set.kind) {
        case DPU_SET_RANKS:
            nr_ranks = dpu_set.list.nr_ranks;
            ranks = dpu_set.list.ranks;
            break;
        case DPU_SET_DPU:
            nr_ranks = 1;
            rank = dpu_get_rank(dpu_set.dpu);
            ranks = &rank;
            break;
    }

    struct dpu_thread_job_sync sync;
    uint32_t nr_jobs_per_rank;
    DPU_THREAD_JOB_GET_JOBS(ranks, nr_ranks, nr_jobs_per_rank, jobs, &sync, policy == DPU_SYNCHRONOUS, status);

    struct dpu_thread_job *job;
    DPU_THREAD_JOB_SET_JOBS(ranks, rank, nr_ranks, jobs, job, &sync, policy == DPU_SYNCHRONOUS, {
        if (dpu_set.kind == DPU_SET_RANKS) {
            job->type = DPU_THREAD_JOB_LAUNCH_RANK;
        } else {
            job->type = DPU_THREAD_JOB_LAUNCH_DPU;
            job->dpu = dpu_set.dpu;
        }
    });

    init_preempt(&dpu_set, &sync, host_func, input, output, length);
    status = dpu_thread_job_do_jobs(ranks, nr_ranks, nr_jobs_per_rank, jobs, policy == DPU_SYNCHRONOUS, &sync);
    clean_preempt(&sync);

    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_launch_preempt(struct dpu_set_t dpu_set, dpu_launch_policy_t policy)
{
    dpu_error_t status = DPU_OK;
    LOG_FN(DEBUG, "%s", dpu_launch_policy_to_string(policy));
    printf("dpu_launch_preempt\n");

    if (dpu_set.kind != DPU_SET_RANKS && dpu_set.kind != DPU_SET_DPU) {
        return DPU_ERR_INTERNAL;
    }

    uint32_t nr_ranks;
    struct dpu_rank_t **ranks;
    struct dpu_rank_t *rank;
    switch (dpu_set.kind) {
        case DPU_SET_RANKS:
            nr_ranks = dpu_set.list.nr_ranks;
            ranks = dpu_set.list.ranks;
            break;
        case DPU_SET_DPU:
            nr_ranks = 1;
            rank = dpu_get_rank(dpu_set.dpu);
            ranks = &rank;
            break;
    }

    struct dpu_thread_job_sync sync;
    uint32_t nr_jobs_per_rank;
    DPU_THREAD_JOB_GET_JOBS(ranks, nr_ranks, nr_jobs_per_rank, jobs, &sync, policy == DPU_SYNCHRONOUS, status);

    struct dpu_thread_job *job;
    DPU_THREAD_JOB_SET_JOBS(ranks, rank, nr_ranks, jobs, job, &sync, policy == DPU_SYNCHRONOUS, {
        if (dpu_set.kind == DPU_SET_RANKS) {
            job->type = DPU_THREAD_JOB_LAUNCH_RANK;
        } else {
            job->type = DPU_THREAD_JOB_LAUNCH_DPU;
            job->dpu = dpu_set.dpu;
        }
    });

    init_preempt(&dpu_set, &sync, NULL, NULL, NULL, 0);
    status = dpu_thread_job_do_jobs(ranks, nr_ranks, nr_jobs_per_rank, jobs, policy == DPU_SYNCHRONOUS, &sync);
    clean_preempt(&sync);

    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_launch(struct dpu_set_t dpu_set, dpu_launch_policy_t policy)
{
    dpu_error_t status = DPU_OK;
    LOG_FN(DEBUG, "%s", dpu_launch_policy_to_string(policy));
    printf("dpu_launch\n");

    if (dpu_set.kind != DPU_SET_RANKS && dpu_set.kind != DPU_SET_DPU) {
        return DPU_ERR_INTERNAL;
    }

    uint32_t nr_ranks;
    struct dpu_rank_t **ranks;
    struct dpu_rank_t *rank;
    switch (dpu_set.kind) {
        case DPU_SET_RANKS:
            nr_ranks = dpu_set.list.nr_ranks;
            ranks = dpu_set.list.ranks;
            break;
        case DPU_SET_DPU:
            nr_ranks = 1;
            rank = dpu_get_rank(dpu_set.dpu);
            ranks = &rank;
            break;
    }

    struct dpu_thread_job_sync sync;
    uint32_t nr_jobs_per_rank;
    DPU_THREAD_JOB_GET_JOBS(ranks, nr_ranks, nr_jobs_per_rank, jobs, &sync, policy == DPU_SYNCHRONOUS, status);

    struct dpu_thread_job *job;
    DPU_THREAD_JOB_SET_JOBS(ranks, rank, nr_ranks, jobs, job, &sync, policy == DPU_SYNCHRONOUS, {
        if (dpu_set.kind == DPU_SET_RANKS) {
            job->type = DPU_THREAD_JOB_LAUNCH_RANK;
        } else {
            job->type = DPU_THREAD_JOB_LAUNCH_DPU;
            job->dpu = dpu_set.dpu;
        }
    });

    status = dpu_thread_job_do_jobs(ranks, nr_ranks, nr_jobs_per_rank, jobs, policy == DPU_SYNCHRONOUS, &sync);

    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_status(struct dpu_set_t dpu_set, bool *done, bool *fault)
{
    LOG_FN(DEBUG, "");

    dpu_sync(dpu_set);

    switch (dpu_set.kind) {
        case DPU_SET_RANKS:
            *done = true;
            *fault = false;

            for (uint32_t each_rank = 0; each_rank < dpu_set.list.nr_ranks; ++each_rank) {
                dpu_error_t status;
                bool rank_done;
                bool rank_fault;

                if ((status = dpu_status_rank(dpu_set.list.ranks[each_rank], &rank_done, &rank_fault)) != DPU_OK) {
                    return status;
                }

                *done = *done && rank_done;
                *fault = *fault || rank_fault;
            }

            return DPU_OK;
        case DPU_SET_DPU:
            return dpu_status_dpu(dpu_set.dpu, done, fault);
        default:
            return DPU_ERR_INTERNAL;
    }
}
