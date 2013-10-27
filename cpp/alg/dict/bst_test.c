#include <stdio.h>
#include <stdlib.h>
#include "bst.h"


#define N_ITEMS 14000
#define RND_MAX 100000


typedef struct test_item {
    int data1;
    int data2;
} test_item_t;


/* item_cmp() - compare the keys of two array items.  This function is passed
 * to the sorting functions.  The return value is negative, zero, or positive
 * depending on whether the array element pointed to by item1 is less than,
 * equal to, or greater than that pointed to by item2.
 *
 * This function must be declared as taking two const void * parameters.
 * Inside the function, the parameters are cast to test_item_t pointers.
 */
int item_cmp(const void *item1, const void *item2)
{
    return ((test_item_t *)item1)->data1 - ((test_item_t *)item2)->data1;
}


int main(void)
{
    bst_t *t;
    test_item_t item_array[N_ITEMS], *i, *i2, duplicate, lookup_item;
    test_item_t *result;
    int j;

    /* Create a binary search tree. */
    t = bst_alloc(item_cmp);
    
    /* Add some items to it. */    
    printf("Inserting items:\n");
    for(j = 0; j < N_ITEMS; j++) {
	i = &item_array[j];
	i->data1 = rand() % RND_MAX;
	i->data2 = rand() % RND_MAX;
        if((i2 = bst_insert(t, i))) {
	    printf("  duplicate %d detected - trying again\n", i2->data1);
	    j--;
	}
	else {
	    printf("  %d", i->data1);
	}
    }

    /* Test duplicate detection. */
    printf("\nAttempting insert to give duplicate key...");
    i2 = &item_array[N_ITEMS/2];
    duplicate.data1 = i2->data1;  duplicate.data2 = rand() % RND_MAX;
    if(bst_insert(t, &duplicate)) {
	printf("duplicate detected.\n");
    }
    else {
	printf("failed - duplicated undected.\n");
	exit(1);
    }
    
    /* Test bst_find() */
    printf("\nTesting bst_find() to find an item...");
    lookup_item.data1 = item_array[N_ITEMS/2].data1;  /* key to lookup. */
    result = bst_find(t, &lookup_item);
    if(result) {
	printf("%s\n", result->data1 == lookup_item.data1
	       ? "successful." : "failed.");
    }
    else {
	printf("failed.\n");
    }

    /* Test bst_find_min() */
    printf("Testing bst_find_min()...");
    result = bst_find_min(t);
    if(result) printf("%d.\n", result->data1); else printf("failed.\n");

    
    /* Test bst_delete(). */
    printf("\nTesting bst_delete()...");
    result = bst_delete(t, &lookup_item);
    if(result) {
	printf("%s\n", result->data1 == lookup_item.data1
	       ? "successful." : "failed.");
    }
    else {
	printf("failed.\n");
    }

    /* Test bst_delete_min() */
    printf("Testing bst_delete_min()...");
    result = bst_delete_min(t);
    if(result) printf("%d.\n", result->data1); else printf("failed.\n");

    /* Test bst_free(). */
    printf("Testing bst_free()...");
    bst_free(t);
    printf("successful.\n");
    
    /* Create a new tree then repeat bst_insert()/bst_delete(). */
    t = bst_alloc(item_cmp);
    printf("\nRepeating bst_insert()/bst_delete()...");
    for(j = 0; j < N_ITEMS; j++) {
	i = &item_array[j];  /* Uniqueness of keys was enforced earlier. */
        bst_insert(t, i);
    }
    for(j = N_ITEMS/2; j < N_ITEMS; j++) {
	i = &item_array[j];
	if(!bst_delete(t, i)) { printf("failed.\n"); exit(1); }
    }
    for(j = N_ITEMS/2; j < N_ITEMS; j++) {
	i = &item_array[j];
        bst_insert(t, i);
    }
    for(j = 0; j < N_ITEMS/2; j++) {
	i = &item_array[j];
	if(!bst_delete(t, i)) { printf("failed.\n"); exit(1); }
    }
    for(j = N_ITEMS/2; j < N_ITEMS; j++) {
	i = &item_array[j];
        if(!bst_delete(t, i)) { printf("failed.\n"); exit(1); }
    }
    printf("successful.\n");

    bst_free(t);
    
    return 0;
}
