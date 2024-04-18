#include "common.h"

struct DataItem* searchItem(HashTable_t* HT, int key) {
    //get the hash 
    int hashIndex = hashFunc(key);

    //move in array until an empty 
    while (HT->hashArray[hashIndex] != NULL) {

        if (HT->hashArray[hashIndex]->key == key)
            return HT->hashArray[hashIndex];

        //go to next cell
        ++hashIndex;

        //wrap around the table
        hashIndex %= SIZE;
    }

    return NULL;
}

void insertItem(HashTable_t* HT, int key, int data) {

    struct DataItem* item = (struct DataItem*)malloc(sizeof(struct DataItem));
    item->data = data;
    item->key = key;

    //get the hash 
    int hashIndex = hashFunc(key);

    //move in array until an empty or deleted cell
    while (HT->hashArray[hashIndex] != NULL && HT->hashArray[hashIndex]->key != -1) {
        //go to next cell
        ++hashIndex;

        //wrap around the table
        hashIndex %= SIZE;
    }

    HT->hashArray[hashIndex] = item;
}

struct DataItem* deleteItem(HashTable_t* HT, struct DataItem* item) {
    int key = item->key;

    //get the hash 
    int hashIndex = hashFunc(key);

    //move in array until an empty
    while (HT->hashArray[hashIndex] != NULL) {

        if (HT->hashArray[hashIndex]->key == key) {
            struct DataItem* temp = HT->hashArray[hashIndex];

            //assign a dummy item at deleted position
            HT->hashArray[hashIndex] = HT->dummyItem;
            return temp;
        }

        //go to next cell
        ++hashIndex;

        //wrap around the table
        hashIndex %= SIZE;
    }

    return NULL;
}

void displayItem(HashTable_t* HT) {
    int i = 0;

    for (i = 0; i < SIZE; i++) {

        if (HT->hashArray[i] != NULL)
            printf(" (%d,%d)", HT->hashArray[i]->key, HT->hashArray[i]->data);
        else
            printf(" ~~ ");
    }

    printf("\n");
}