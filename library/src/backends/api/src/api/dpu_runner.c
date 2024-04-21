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
#include <dpu_internals.h>

#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#include <dpu_program.h>

#define RANK_THRESHOLD 0.4
#define EXECUTION_WINDOW 0.02

struct preempt_info{
    pthread_t preempt_thread;
    struct dpu_set_t* dpu_set;
    struct dpu_thread_job_sync* sync;

    long long avg_execution_dur;
    int nr_rank;
    int nr_rank_threshold;
    short* rank_done;
    short* prev_done;
    long long check_interval;
    long long* execution_times;
    struct timeval startTime;
    int num_done;
    int* abort;

    // input, query, output, data length, index
    void (*host_func)(uint32_t);
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

static void each_rank_status_2() {
    struct dpu_set_t dpu;
    uint32_t each_dpu;

    DPU_FOREACH(*(preempt.dpu_set), dpu, each_dpu)
    {
        DPU_ASSERT(dpu_prepare_xfer(dpu, &(preempt.rank_done[each_dpu])));
    }
    DPU_ASSERT(dpu_push_xfer(*(preempt.dpu_set), DPU_XFER_FROM_DPU, "finish_regular_work", 0, sizeof(int), DPU_XFER_DEFAULT));

    for (int i = 0; i < preempt.nr_rank; i++) {
        printf("dpu %d status %d %d\n", i, preempt.rank_done[i], preempt.prev_done[i]);
        if (preempt.rank_done[i]) {
            if (!preempt.prev_done[i]) {
                long long elapsedTime = getElaspedTime();
                preempt.prev_done[i] = 1;
                preempt.execution_times[i] = elapsedTime;

                preempt.avg_execution_dur = preempt.avg_execution_dur * preempt.num_done + elapsedTime;
                preempt.num_done ++;
                preempt.avg_execution_dur /= preempt.num_done;
            }
        }  
    }
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

void drain_each_dpu(int rankIdx) {
    struct dpu_rank_t *rank = preempt.dpu_set->list.ranks[rankIdx];
    uint8_t nr_dpus = preempt.dpu_set->list.ranks[rankIdx]->nr_dpus_enabled;

    for (int each_dpu = 0; each_dpu < nr_dpus; ++each_dpu) {
        struct dpu_t *dpu = rank->dpus + each_dpu;
        struct dpu_context_t dpu_context;
        dpu_context_fill_from_rank(&dpu_context, rank);
        // drain_pipeline(dpu, &dpu_context, false);
    }
}

void abort_struggling_dpu(int rankIdx) {
    printf("abort_struggling_dpu %d\n", rankIdx);
    struct dpu_rank_t* rank = preempt.dpu_set->list.ranks[rankIdx];
    
    rank->api.thread_info.should_stop = true;
    rank->api.abort = true;

    // dpu_run_context_t run_context = dpu_get_run_context(rank);
    // while (run_context->nb_dpu_running != 0) {
    //     run_context->nb_dpu_running = 0;
    //     pthread_cond_broadcast(&rank->api.poll_cond);
    // }

    dpu_reset_rank(preempt.dpu_set->list.ranks[rankIdx]);

    if (preempt.host_func != NULL) {
        preempt.host_func(rankIdx);
    }
}

void abort_struggling_dpus() {
    printf("abort_struggling_dpus\n"); 
    for (int i = 0; i < preempt.nr_rank; i++) {
        if (!preempt.rank_done[i]) {
            preempt.abort[i] = 1;
            abort_struggling_dpu(i);
        } else {
            preempt.abort[i] = 0;
        }
    }
}

void enable_stealing() {
    printf("enable_stealing\n");
    int stealing = 1;
    struct dpu_set_t dpu;
    uint32_t each_dpu;

    DPU_FOREACH(*(preempt.dpu_set), dpu, each_dpu)
    {
        DPU_ASSERT(dpu_prepare_xfer(dpu, &(stealing)));
    }
    DPU_ASSERT(dpu_push_xfer(*(preempt.dpu_set), DPU_XFER_TO_DPU, "start_stealing", 0, sizeof(int), DPU_XFER_DEFAULT));
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
            break;
        }
    }

    if (abort) {
        abort_struggling_dpus();
        // enable_stealing();
    }
    return NULL;
}

static void init_preempt(struct dpu_set_t * dpu_set, struct dpu_thread_job_sync * sync,
                            void (*host_func)(uint32_t), int* abort) {
    // dpu_get_nr_dpus(*dpu_set, &preempt.nr_rank);
    preempt.nr_rank = sync->nr_ranks;
    preempt.check_interval = 50000000L;
    preempt.dpu_set = dpu_set;
    preempt.num_done = 0;
    preempt.avg_execution_dur = 0;
    preempt.nr_rank_threshold = RANK_THRESHOLD * preempt.nr_rank;
    preempt.host_func = host_func;
    preempt.abort = abort;
    preempt.sync = sync;

    preempt.execution_times = malloc(sizeof(long long) * preempt.nr_rank);
    memset(preempt.execution_times, 0, sizeof(long long) * preempt.nr_rank);
    preempt.rank_done = malloc(sizeof(short) * preempt.nr_rank);
    memset(preempt.rank_done, 0, sizeof(short) * preempt.nr_rank);
    preempt.prev_done = malloc(sizeof(short) * preempt.nr_rank);
    memset(preempt.prev_done, 0, sizeof(short) * preempt.nr_rank);

    sync->stop_polling = false;
    int ret = pthread_create(&preempt.preempt_thread, NULL, dpu_poll_status, (void*)sync);
}

static void clean_preempt(struct dpu_thread_job_sync * sync) {
    sync->stop_polling = true;
    sync->nr_ranks = 0;
    pthread_join(preempt.preempt_thread, NULL);
    free(preempt.execution_times);
    free(preempt.rank_done);
    free(preempt.prev_done);
}

__API_SYMBOL__ dpu_error_t
dpu_launch_preempt_restart(struct dpu_set_t dpu_set, dpu_launch_policy_t policy,
                    void (*host_func)(uint32_t), int* abort)
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

    init_preempt(&dpu_set, &sync, host_func, abort);
    status = dpu_thread_job_do_jobs(ranks, nr_ranks, nr_jobs_per_rank, jobs, policy == DPU_SYNCHRONOUS, &sync);
    clean_preempt(&sync);

    return status;
}

__API_SYMBOL__ dpu_error_t
dpu_launch_preempt(struct dpu_set_t dpu_set, dpu_launch_policy_t policy, int* abort)
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

    init_preempt(&dpu_set, &sync, NULL, abort);
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
