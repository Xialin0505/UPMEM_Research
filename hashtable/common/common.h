#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "stdlib.h"

#define XSTR(x) STR(x)
#define STR(x) #x

/* DPU variable that will be read of write by the host */
#define DPU_BUFFER dpu_mram_buffer

#define DPU_CACHES dpu_wram_caches
#define DPU_RESULTS dpu_wram_results

#define BUFFER_SIZE (8 << 20)
#define TABLE_SIZE (BUFFER_SIZE / 8)

/* Use blocks of 256 bytes */
#define BLOCK_SIZE (256)

typedef struct
{
    int32_t hashsum;
    int32_t cycles;
    int32_t idx;
} dpu_result_t;

typedef struct
{
    dpu_result_t tasklet_result[NR_TASKLETS];
} dpu_results_t;

typedef struct DataItem
{
    int32_t data;
    int32_t key;
} hash_t;

typedef struct {
    hash_t hash_array[TABLE_SIZE];
} table_t;

// // Hash function
// static int32_t hash(int32_t key) {
//     return key % TABLE_SIZE;
// }

// // Initialize hash table
// void init_hash_table(int8_t *used, int32_t size) {
//     for (int32_t i = 0; i < size; i++) {
//         *(used + i) = 0;
//     }
// }

// // Insert function
// void insert(int32_t key, int32_t value, int32_t *keys, int32_t *values, int8_t *used) {
//     int32_t index = hash(key);
//     while (*(used + index) == 1 && *(keys + index) != key) {
//         index = (index + 1) % TABLE_SIZE;  // Linear probing
//     }
//     *(keys + index) = key;
//     *(values + index) = value;
//     *(used + index) = 1;
// }

// // Search function
// int32_t search(int32_t key, const int32_t *keys, int32_t *values, const int8_t *used) {
//     int index = hash(key);
//     while (*(used + index)) {
//         if (*(keys + index) == key) {
//             return *(values + index);  // Return the corresponding value
//         }
//         index = (index + 1) % TABLE_SIZE;  // Linear probing
//     }
//     return -1;  // Key not found
// }

void inith(table_t * table) {
    printf("init all key to -1\n");
    int i;
    for (i = 0; i < TABLE_SIZE; i++) {
        table->hash_array[i].key = -1;
        table->hash_array[i].data = 0;
    }
}

table_t * createTable() {
    table_t *table = malloc(sizeof(table_t));
    inith(table);
    return table;
}

int hashCode(int key)
{
    return key % TABLE_SIZE;
}

int32_t searchItem(table_t* table, int key)
{
    // get the hash
    int hashIndex = hashCode(key);
    int it = 0;
    // printf("search item\n");
    // move in array until an empty
    while (((table->hash_array)[hashIndex]).key != -1)
    {

        if (((table->hash_array)[hashIndex]).key == key)
            return ((table->hash_array)[hashIndex]).data;

        if (it == TABLE_SIZE) {
            break;
        }
        // go to next cell
        ++hashIndex;
        it++;

        // wrap around the table
        hashIndex %= TABLE_SIZE;
    }

    

    return 0;
}

void insertItem(table_t* table, int key, int data)
{
    // get the hash
    int hashIndex = hashCode(key);
    // printf("hashcode = %d\n", hashIndex);
    int it = 0;
    // printf("insert item\n");
    // move in array until an empty or deleted cell
    // while (((table->hash_array)[hashIndex]).key != -1)
    // {
    //     if (it == TABLE_SIZE) {
    //         return;
    //     }
    //     // go to next cell
    //     ++hashIndex;
    //     it++;

    //     // wrap around the table
    //     hashIndex %= TABLE_SIZE;
    // }

    // ((table->hash_array)[hashIndex]).key = key;
    // ((table->hash_array)[hashIndex]).data = data;
    // printf("interted, k = %d, d = %d\n", ((table->hash_array)[hashIndex]).key, ((table->hash_array)[hashIndex]).data);

    for (it = 0; it < TABLE_SIZE; it++, hashIndex++) {
        if (((table->hash_array)[hashIndex]).key == -1) {
            // printf("hashIdx = %d\n", hashIndex);
            ((table->hash_array)[hashIndex]).key = key;
            ((table->hash_array)[hashIndex]).data = data;
            // printf("IIIII, k = %d, d = %d\n", key, data);
            break;
        }
        
        hashIndex %= TABLE_SIZE;
    }
    // printf("interted, k = %d, d = %d\n", ((table->hash_array)[hashIndex]).key, ((table->hash_array)[hashIndex]).data);

}

// void initHashTable(struct DataItem *hashArray)
// {
//     for (int i = 0; i < TABLE_SIZE; i++)
//     {
//         (*(hashArray + i)).key = -1;
//         (*(hashArray + i)).data = -1;
//     }
// }

// int32_t searchItem(struct DataItem *hashArray, int key)
// {
//     // get the hash
//     int hashIndex = hashCode(key);
//     int it = 0;
//     // printf("search item\n");
//     // move in array until an empty
//     while ((*(hashArray + hashIndex)).key != -1)
//     {

//         if ((*(hashArray + hashIndex)).key == key)
//             return (*(hashArray + hashIndex)).data;

//         if (it == TABLE_SIZE) {
//             break;
//         }
//         // go to next cell
//         ++hashIndex;
//         it++;

//         // wrap around the table
//         hashIndex %= TABLE_SIZE;
//     }

//     return -1;
// }

// void insertItem(struct DataItem *hashArray, int key, int data)
// {
//     // get the hash
//     int hashIndex = hashCode(key);
//     int it = 0;
//     // printf("insert item\n");
//     // move in array until an empty or deleted cell
//     while ((*(hashArray + hashIndex)).key != -1)
//     {
//         if (it == TABLE_SIZE) {
//             return;
//         }
//         // go to next cell
//         ++hashIndex;
//         it++;

//         // wrap around the table
//         hashIndex %= TABLE_SIZE;
//     }

//     (*(hashArray + hashIndex)).key = key;
//     (*(hashArray + hashIndex)).data = data;
// }

// struct DataItem* deleteItem(struct DataItem* hashArray[], struct DataItem* item) {
//    int key = item->key;

//    //get the hash
//    int hashIndex = hashCode(key);

//    //move in array until an empty
//    while(hashArray[hashIndex] != NULL) {

//       if(hashArray[hashIndex]->key == key) {
//          struct DataItem* temp = hashArray[hashIndex];

//          //assign a dummy item at deleted position
//          hashArray[hashIndex]->key = -1;
//          hashArray[hashIndex]->data = -1;
//          return temp;
//       }

//       //go to next cell
//       ++hashIndex;

//       //wrap around the table
//       hashIndex %= TABLE_SIZE;
//    }

//    return NULL;
// }

#endif /* __COMMON_H__ */
