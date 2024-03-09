/* Copyright 2020 UPMEM. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <dpu.h>
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
#include <unistd.h>
#include <time.h>

struct preempt_info{
    pthread_t preempt_thread;
    struct dpu_set_t* dpu_set;

    double avg_execution_dur;
    int nr_rank;
    int nr_rank_threshold;
    short* rank_done;
    long long check_interval;
    long long* execution_times;
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

/*
output the 
*/
static void determine_struggle_dpu(struct preempt_info * arg) {

}

static void each_rank_status() {
    for (uint32_t each_rank = 0; each_rank < preempt.nr_rank; ++each_rank) {
        dpu_error_t status;
        bool rank_done;
        bool rank_fault;

        if ((status = dpu_status_rank(preempt.dpu_set->list.ranks[each_rank], &rank_done, &rank_fault)) != DPU_OK) {
            return status;
        }

        if (rank_done) {
            preempt.rank_done[each_rank] = 1;
        } else if (rank_fault) {
            preempt.rank_done[each_rank] = 2;
        } else {
            preempt.rank_done[each_rank] = 0;
        }
    }
}

static void print_rank_status() {
    for (int i = 0; i < preempt.nr_rank; i++) {
        printf("done %d ", preempt.rank_done[i]);
    }
    printf("\n");
}

void * dpu_poll_status(void * arg) {
    struct dpu_thread_job_sync * sync = (struct dpu_thread_job_sync*) arg;
    printf("total nr_rank %d\n", preempt.nr_rank);

    while (!sync->stop_polling) {
        int running_rank_num = sync -> nr_ranks;
        nano_sleep(preempt.check_interval);
        // printf("running nr_rank %d\n", running_rank_num);
        each_rank_status();
        print_rank_status();
    }

}

static void init_preempt(struct dpu_set_t * dpu_set, struct dpu_thread_job_sync * sync) {
    preempt.nr_rank = sync->nr_ranks;
    preempt.check_interval = 5000000L;
    preempt.dpu_set = dpu_set;
    
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

// __API_SYMBOL__ dpu_error_t
// dpu_launch(struct dpu_set_t dpu_set, dpu_launch_policy_t policy)
// {
//     dpu_error_t status = DPU_OK;
//     LOG_FN(DEBUG, "%s", dpu_launch_policy_to_string(policy));

//     if (dpu_set.kind != DPU_SET_RANKS && dpu_set.kind != DPU_SET_DPU) {
//         return DPU_ERR_INTERNAL;
//     }

//     uint32_t nr_ranks;
//     struct dpu_rank_t **ranks;
//     struct dpu_rank_t *rank;
//     switch (dpu_set.kind) {
//         case DPU_SET_RANKS:
//             nr_ranks = dpu_set.list.nr_ranks;
//             ranks = dpu_set.list.ranks;
//             break;
//         case DPU_SET_DPU:
//             nr_ranks = 1;
//             rank = dpu_get_rank(dpu_set.dpu);
//             ranks = &rank;
//             break;
//     }

//     struct dpu_thread_job_sync sync;
//     uint32_t nr_jobs_per_rank;
//     DPU_THREAD_JOB_GET_JOBS(ranks, nr_ranks, nr_jobs_per_rank, jobs, &sync, policy == DPU_SYNCHRONOUS, status);

//     struct dpu_thread_job *job;
//     DPU_THREAD_JOB_SET_JOBS(ranks, rank, nr_ranks, jobs, job, &sync, policy == DPU_SYNCHRONOUS, {
//         if (dpu_set.kind == DPU_SET_RANKS) {
//             job->type = DPU_THREAD_JOB_LAUNCH_RANK;
//         } else {
//             job->type = DPU_THREAD_JOB_LAUNCH_DPU;
//             job->dpu = dpu_set.dpu;
//         }
//     });

//     status = dpu_thread_job_do_jobs(ranks, nr_ranks, nr_jobs_per_rank, jobs, policy == DPU_SYNCHRONOUS, &sync);

//     return status;
// }

__API_SYMBOL__ dpu_error_t
dpu_launch(struct dpu_set_t dpu_set, dpu_launch_policy_t policy)
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

    init_preempt(&dpu_set, &sync);

    status = dpu_thread_job_do_jobs(ranks, nr_ranks, nr_jobs_per_rank, jobs, policy == DPU_SYNCHRONOUS, &sync);
    
    clean_preempt(&sync);

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
