/*** File: hash_test.c - Example program for the hash table implementation ***/
/*
 *   Shane Saunders
 */

#include <stdio.h>
#include <stdlib.h>
#include "hashtbl.h"


#define N_ITEMS 150
#define N_BUCKETS 13


/* Structure type for items. */
typedef struct test_item {
    int key;
    int other_data;
} test_item_t;


/* Function to compare two items. */
int compare_fn(const void *item1, const void *item2)
{
    return ((test_item_t *)item1)->key - ((test_item_t *)item2)->key;
}

/* Hash function to use. */
int hash_fn(const void *item, int n)
{
    /* Generate the hash value by using the key as a seed for the random number
     * generator, and taking the second random number generated.
     */
    srand(((test_item_t *)item)->key);
    rand();
    return rand() % n;
}


/* Main Program */
int main(void)
{
    int i;
    test_item_t items[N_ITEMS];
    test_item_t lookup, duplicate;
    hash_tbl_t *hash_table;

    /* Assign unique keys to the test items.  For item i we will use the key
     * i^2.
     */
    for(i = 0; i < N_ITEMS; i++) {
	items[i].key = i*i;
    }

    /* Create a has table with N_BUCKETS buckets. */
    hash_table = hash_tbl_alloc(N_BUCKETS, hash_fn, compare_fn);

    /* Insert all the items into the hash table. */
    printf("Testing inserting items...");
    for(i = 0; i < N_ITEMS; i++) {
        if(hash_tbl_insert(hash_table, &items[i]) != NULL) {
            printf("error.\n");
            exit(1);
	}
    }
    printf("passed.\n");

    /* Try inserting an item which duplicates an existing items key. */
    printf("Testing key duplication...");
    duplicate.key = 2*2;
    if(hash_tbl_insert(hash_table, &duplicate) != &items[2]) {
        printf("error.\n");
        exit(1);
    }
    printf("passed.\n");

    /* Try finding items with certain keys. */
    printf("Testing find...\n");
    for(i = 0; i < N_ITEMS; i++) {
	lookup.key = i;
        if(hash_tbl_find(hash_table, &lookup)) {
            printf("  key %d - found\n", i);
	}
	else {
            printf("  key %d - not found\n", i);
	}
    }

    /* Now try deleting items. */
    printf("Testing delete...");
    for(i = 0; i < N_ITEMS; i++) {
	lookup.key = i*i;
        if(hash_tbl_delete(hash_table, &lookup) != &items[i]) {
	    printf("error.\n");
	    exit(1);
	}
    }
    printf("passed.\n");

    return 0;
}
