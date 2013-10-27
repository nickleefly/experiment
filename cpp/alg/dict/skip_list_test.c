#include <stdio.h>
#include <stdlib.h>
#include "skip_list.h"


/* #define DEBUG */
#define N_ITEMS 1500
#define RND_MAX 10000


typedef struct test_item {
    int data1;
    int data2;
} test_item_t;


/* Funtion for indenting output. */
void spaces(int l)
{
    int i;
    for(i = 0; i < l; i++) {
        putchar(' ');
    }
}

    
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
    skip_list_t *t;
    test_item_t item_array[N_ITEMS], *i, *i2, duplicate, lookup_item;
    test_item_t *result;
    int j;


    /* Create a skip list. */
    t = skip_list_alloc(N_ITEMS, 0.5, item_cmp);
    
    /* Add some items to it.  In this example, items have two fields, data1 and
     * data2.  The data1 field is used as the items key (see the item
     * comparison function, item_cmp(), defined above.
     */ 
    printf("Inserting items:\n");
    for(j = 0; j < N_ITEMS; j++) {
	i = &item_array[j];
	i->data1 = rand() % RND_MAX;
	i->data2 = rand() % RND_MAX;
        if((i2 = skip_list_insert(t, i))) {
	    printf("  duplicate %d detected - trying again\n", i2->data1);
	    j--;
	}
	else {
	    printf("Inserted key %d:\n", i->data1);
	}
    }

    /* Test duplicate detection. */
    printf("\nAttempting insert to give duplicate key...");
    i2 = &item_array[N_ITEMS/2];
    duplicate.data1 = i2->data1;  duplicate.data2 = rand() % RND_MAX;
    if(skip_list_insert(t, &duplicate)) {
	printf("duplicate detected.\n");
    }
    else {
	printf("failed - duplicate undetected.\n");
	exit(1);
    }
    
    /* Test skip_list_find() */
    printf("\nTesting skip_list_find() to find an item...");
    lookup_item.data1 = item_array[N_ITEMS/2].data1;  /* key to lookup. */
    result = skip_list_find(t, &lookup_item);
    if(result) {
	printf("%s\n", result->data1 == lookup_item.data1
	       ? "successful." : "failed.");
    }
    else {
	printf("failed.\n");
    }

    /* Test skip_list_find_min() */
    printf("Testing skip_list_find_min()...");
    result = skip_list_find_min(t);
    if(result) printf("%d.\n", result->data1); else printf("failed.\n");

    
    /* Test skip_list_delete(). */
    printf("\nTesting skip_list_delete()...");
    result = skip_list_delete(t, &lookup_item);
    if(result) {
	printf("%s\n", result->data1 == lookup_item.data1
	       ? "successful." : "failed.");
    }
    else {
	printf("failed.\n");
    }

    /* Test skip_list_delete_min() */
    printf("Testing skip_list_delete_min()...");
    result = skip_list_delete_min(t);
    if(result) printf("%d.\n", result->data1); else printf("failed.\n");


    /* Test skip_list_free(). */
    printf("Testing skip_list_free()...");
    skip_list_free(t);
    printf("successful.\n");

    
    /* Create a new tree then repeat skip_list_insert()/skip_list_delete(). */
    t = skip_list_alloc(N_ITEMS, 0.5, item_cmp);
    printf("\nRepeating skip_list_insert()/skip_list_delete()...");
    for(j = 0; j < N_ITEMS; j++) {
	i = &item_array[j];  /* Uniqueness of keys was enforced earlier. */
	printf("Inserting key %d:\n", i->data1);
        skip_list_insert(t, i);
    }
    for(j = N_ITEMS/2; j < N_ITEMS; j++) {
	i = &item_array[j];
	printf("Deleting key %d:\n", i->data1);
	if(!skip_list_delete(t, i)) { printf("failed.\n"); exit(1); }
    }
    for(j = N_ITEMS/2; j < N_ITEMS; j++) {
	i = &item_array[j];
	printf("Inserting key %d:\n", i->data1);
        skip_list_insert(t, i);
    }
    for(j = 0; j < N_ITEMS/2; j++) {
	i = &item_array[j];
	printf("Deleting key %d:\n", i->data1);
	if(!skip_list_delete(t, i)) { printf("failed.\n"); exit(1); }
    }
    for(j = N_ITEMS/2; j < N_ITEMS; j++) {
	i = &item_array[j];
	printf("Deleting key %d:\n", i->data1);
        if(!skip_list_delete(t, i)) { printf("failed.\n"); exit(1); }
    }
    printf("successful.\n");    

    /* Test delete_min. */
    for(j = 0; j < N_ITEMS; j++) {
	i = &item_array[j];  /* Uniqueness of keys was enforced earlier. */
	printf("Inserting key %d:\n", i->data1);
        skip_list_insert(t, i);
    }
    for(j = 0; j < N_ITEMS; j++) {
        if(!(i = skip_list_delete_min(t))) { printf("failed.\n"); exit(1); }
	printf("Deleted minimum key %d:\n", i->data1);
    }
    skip_list_free(t);
    
    return 0;
}
