#include <dpu.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "dict.h"
#include "connect.h"
#ifndef DPU_BINARY
#define DPU_BINARY "./dpu_task"
#endif

void printRequestSizes(request_batch *rqst)
{
    for (int i = 0; i < NR_DPUS; i++)
    {
        printf("%d ", rqst->size[i]);
    }
    printf("\n");
}

void setTraces(dict *dt, request_batch *rqst)
{
    dictAdd(dt, rqst, "kkk", "kkkk");
    dictFind(dt, rqst, "kkk");
    dictReplace(dt, rqst, "kkk", "KKKK");
    dictFind(dt, rqst, "kkk");
    dictDelete(dt, rqst, "kkk");
    dictFind(dt, rqst, "kkk");

    // dictAdd(dt, rqst, "abc", "abcd");
    // dictFind(dt, rqst, "abc");
    // dictReplace(dt, rqst, "abc", "ABCD");
    // dictFind(dt, rqst, "abc");
    // dictDelete(dt, rqst, "abc");
    // dictFind(dt, rqst, "abc");

    // dictAdd(dt, rqst, "xxx", "asdsa");
    // dictFind(dt, rqst, "xxx");
    // dictReplace(dt, rqst, "xxx", "ASDSA");
    // dictFind(dt, rqst, "xxx");
    // dictDelete(dt, rqst, "xxx");
    // dictFind(dt, rqst, "xxx");

    // // Test case 1: Add, replace, and find operations with a long string
    // dictAdd(dt, rqst, "longKey1", "aVeryLongString");
    // dictFind(dt, rqst, "longKey1");
    // dictReplace(dt, rqst, "longKey1", "AnotherVery");
    // dictFind(dt, rqst, "longKey1");
    // dictDelete(dt, rqst, "longKey1");
    // dictFind(dt, rqst, "longKey1");

    // // Test case 2: Multiple adds without deletes to check for memory leaks or overwrites
    // dictAdd(dt, rqst, "multiKey", "value1");
    // dictAdd(dt, rqst, "multiKey", "value2");
    // dictFind(dt, rqst, "multiKey");
    // dictReplace(dt, rqst, "multiKey", "value3");
    // dictFind(dt, rqst, "multiKey");
    // dictDelete(dt, rqst, "multiKey");
    // dictFind(dt, rqst, "multiKey");

    // // Test case 3: Adding a large number of unique keys to test scaling and performance
    // for (int i = 0; i < 10; i++)
    // {
    //     char key[20], value[20];
    //     sprintf(key, "key%d", i);
    //     sprintf(value, "value%d", i);
    //     dictAdd(dt, rqst, key, value);
    // }

    // // Ensure all keys are findable
    // for (int i = 0; i < 10; i++)
    // {
    //     char key[20];
    //     sprintf(key, "key%d", i);
    //     dictFind(dt, rqst, key);
    // }

    // // Delete all keys
    // for (int i = 0; i < 10; i++)
    // {
    //     char key[20];
    //     sprintf(key, "key%d", i);
    //     dictDelete(dt, rqst, key);
    // }

    // // Test case 4: Stress test with rapid adds and deletes
    // dictAdd(dt, rqst, "tempKey", "tempValue");
    // for (int i = 0; i < 10; i++)
    // {
    //     dictReplace(dt, rqst, "tempKey", "newValue");
    //     dictDelete(dt, rqst, "tempKey");
    //     dictAdd(dt, rqst, "tempKey", "tempValue");
    // }
    // dictFind(dt, rqst, "tempKey");
    // dictDelete(dt, rqst, "tempKey");

}

int main()
{
    struct dpu_set_t set, dpu;
    uint32_t nr_dpus = NR_DPUS;
    uint32_t nr_of_dpus;
    uint32_t each_dpu;
    request_batch *rqst = requestInit();
    response_batch *rpse = responseInit();
    dict *dt = dictInit();

    setTraces(dt, rqst);

    printRequestSizes(rqst);

    DPU_ASSERT(dpu_alloc(nr_dpus, NULL, &set));
    DPU_ASSERT(dpu_load(set, DPU_BINARY, NULL));
    DPU_ASSERT(dpu_get_nr_dpus(set, &nr_of_dpus));
    printf("Allocated %d DPU(s)\n", nr_of_dpus);

    uint32_t num_rank = 0;
    dpu_get_nr_ranks(set, &num_rank);
    printf("Allocated %d rank(s)\n", num_rank);
    uint32_t *alter_result = malloc(num_rank * sizeof(uint32_t));
    DPU_FOREACH(set, dpu, each_dpu)
    {
        DPU_ASSERT(dpu_prepare_xfer(dpu, &(rqst->size[each_dpu])));
    }
    DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_TO_DPU, "rqst_size", 0, sizeof(unsigned int), DPU_XFER_DEFAULT));

    DPU_FOREACH(set, dpu, each_dpu)
    {
        DPU_ASSERT(dpu_prepare_xfer(dpu, &(rqst->rqst[each_dpu * BATCH_SIZE])));
    }
    DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_TO_DPU, "rqst", 0, sizeof(request) * BATCH_SIZE, DPU_XFER_DEFAULT));

    // DPU_ASSERT(dpu_broadcast_to(set, "rqst", 0, rqst, sizeof(request) * BATCH_SIZE, DPU_XFER_DEFAULT));
    DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));

    DPU_FOREACH(set, dpu, each_dpu)
    {
        DPU_ASSERT(dpu_prepare_xfer(dpu, &(rpse->size[each_dpu])));
    }
    DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_FROM_DPU, "rpse_size", 0, sizeof(unsigned int), DPU_XFER_DEFAULT));

    DPU_FOREACH(set, dpu, each_dpu)
    {
        DPU_ASSERT(dpu_prepare_xfer(dpu, &(rpse->rpse[each_dpu * BATCH_SIZE])));
    }
    DPU_ASSERT(dpu_push_xfer(set, DPU_XFER_FROM_DPU, "rpse", 0, sizeof(response) * BATCH_SIZE, DPU_XFER_DEFAULT));
    DPU_FOREACH(set, dpu, each_dpu)
    {
        printf("dpu:[%u]\n", each_dpu);
        for (int i = each_dpu * BATCH_SIZE; i < each_dpu * BATCH_SIZE + rpse->size[each_dpu]; i++)
        {
            if (rpse->rpse[i].response == DICT_ADD_OK)
                dt->used++;
            if (rpse->rpse[i].response == DICT_DELETE_OK)
                dt->used--;
            reponsePrint(&(rpse->rpse[i]));
            // printf("in dpu: %u \n", each_dpu);
        }
    }

    DPU_ASSERT(dpu_free(set));
    return 0;
}