#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dict.h"

static unsigned int _dictNextPower(unsigned int size);
static int _dictKeyIndex(dict *ht, const char *key);

/* -------------------------- hash functions -------------------------------- */

/* Thomas Wang's 32 bit Mix Function */
unsigned int dictIntHashFunction(unsigned int key)
{
    key += ~(key << 15);
    key ^= (key >> 10);
    key += (key << 3);
    key ^= (key >> 6);
    key += ~(key << 11);
    key ^= (key >> 16);
    return key;
}

/* Generic hash function (a popular one from Bernstein).
 * I tested a few and this was the best. */
unsigned int dictGenHashFunction(const char *buf, int len)
{
    unsigned int hash = 5381;

    while (len--)
        hash = ((hash << 5) + hash) + (*buf++); /* hash * 33 + c */
    return hash;
}

/* ----------------------------- API implementation ------------------------- */
/* Initialize the hash table */
dict *dictInit()
{
    dict *ht = malloc(sizeof(dict));
    ht->size = DICT_HT_INITIAL_SIZE;
    ht->sizemask = ht->size - 1;
    ht->used = 0;
    return ht;
}

/* Add an element to the target hash table */
int dictAdd(dict *ht, request_batch *rqst, const char *key_, const char *val_)
{
    printf("\ndictAdd\n");
    int index;

    index = _dictKeyIndex(ht, key_);
    // printf("ADD -- index = %d\n", index);
    int dpu_num = index / DICT_HT_INITIAL_SIZE_PER_PDU;
    int bucket = index % DICT_HT_INITIAL_SIZE_PER_PDU;
    int loop_counter = 0;
    while (rqst->size[dpu_num] != 0 && loop_counter < NR_DPUS) {  // find empty dpu
        printf("HT Key: %s ; key_: %s \n", rqst->rqst[dpu_num * BATCH_SIZE].key.buf, key_);
        dpu_num++;
        loop_counter++;
        if (dpu_num > NR_DPUS) {
            dpu_num = 0;
        }
    }

    requestAdd(rqst, dpu_num, INSERT, bucket, key_, val_);
    
    return DICT_OK;
}

/* Add an element, discarding the old if the key already exists */
int dictReplace(dict *ht, request_batch *rqst, const char *key_, const char *val_)
{
    printf("\ndictReplace\n");
    int index;

    index = _dictKeyIndex(ht, key_);
    // printf("REP -- index = %d\n", index);

    int dpu_num = index / DICT_HT_INITIAL_SIZE_PER_PDU;
    int bucket = index % DICT_HT_INITIAL_SIZE_PER_PDU;
    int loop_counter = 0;
    while (rqst->rqst[dpu_num * BATCH_SIZE].key.buf != key_ && loop_counter < NR_DPUS) {  // find dpu with the key
        printf("HT Key: %s ; key_: %s \n", rqst->rqst[dpu_num * BATCH_SIZE].key.buf, key_);
        dpu_num++;
        loop_counter++;
        if (dpu_num > NR_DPUS) {
            dpu_num = 0;
        }
    }
    requestAdd(rqst, dpu_num, REPLACE, bucket, key_, val_);
    return DICT_OK;
}

int dictDelete(dict *ht, request_batch *rqst, const char *key_)
{
    printf("\ndictDelete\n");
    int index;

    index = _dictKeyIndex(ht, key_);

    // printf("DEL -- index = %d\n", index);

    int dpu_num = index / DICT_HT_INITIAL_SIZE_PER_PDU;
    int bucket = index % DICT_HT_INITIAL_SIZE_PER_PDU;
    int loop_counter = 0;
    while (rqst->rqst[dpu_num * BATCH_SIZE].key.buf != key_ && loop_counter < NR_DPUS) {  // find dpu with the key
        printf("HT Key: %s ; key_: %s \n", rqst->rqst[dpu_num * BATCH_SIZE].key.buf, key_);
        dpu_num++;
        loop_counter++;
        if (dpu_num > NR_DPUS) {
            dpu_num = 0;
        }
    }
    requestAdd(rqst, dpu_num, DELETE, bucket, key_, NULL);
    return DICT_OK;
}

int dictFind(dict *ht, request_batch *rqst, const char *key_)
{
    printf("\ndictFind\n");
    int index;

    index = _dictKeyIndex(ht, key_);

    // printf("FIND -- index = %d\n", index);

    int dpu_num = index / DICT_HT_INITIAL_SIZE_PER_PDU;
    int bucket = index % DICT_HT_INITIAL_SIZE_PER_PDU;
    
    int loop_counter = 0;
    if (rqst->rqst[dpu_num * BATCH_SIZE].key.buf == key_) {
        printf("same xxxxxxxxxxxxxxxxxxxxxxxxxxx");
    }
    while (rqst->rqst[dpu_num * BATCH_SIZE].key.buf != key_ && loop_counter < NR_DPUS) {  // find dpu with the key
        printf("HT Key: -%s- ; key_: -%s- \n", rqst->rqst[dpu_num * BATCH_SIZE].key.buf, key_);
        dpu_num++;
        loop_counter++;
        if (dpu_num > NR_DPUS) {
            dpu_num = 0;
        }
    }

    requestAdd(rqst, dpu_num, FIND, bucket, key_, NULL);
    return DICT_OK;
}
/* ------------------------- private functions ------------------------------ */
/* Our hash table capability is a power of two */
static unsigned int _dictNextPower(unsigned int size)
{
    unsigned int i = DICT_HT_INITIAL_SIZE;

    if (size >= 2147483648U)
        return 2147483648U;
    while (1)
    {
        if (i >= size)
            return i;
        i *= 2;
    }
}

/* Returns the index of a free slot that can be populated with
 * an hash entry for the given 'key'.
 * If the key already exists, -1 is returned. */
static int _dictKeyIndex(dict *ht, const char *key)
{
    unsigned int h;

    /* Compute the key hash value */
    h = dictGenHashFunction(key, strlen(key)) & ht->sizemask;

    return h;
}
