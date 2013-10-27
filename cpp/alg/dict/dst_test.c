#include <stdio.h>
#include <stdlib.h>
#include "dst.h"


#define N_ITEMS 12
#define RND_MAX 64
#define MAX_BITS 6




typedef struct test_item {
    int data1;
    int data2;
} test_item_t;


void dst_dump(dst_node_t *p, int l) {
    int i;
    
    for(i = 0; i < 2*l; i++) putchar(' ');

    if(p) printf("%d\n", ((test_item_t *)p->item)->data1);
    else { printf("--\n"); return; }
    
    if(!(p->a[0] || p->a[1])) return;

    dst_dump(p->a[0], l+1);
    dst_dump(p->a[1], l+1);
}


/* item_value() - compare the keys of two array items.  This function is passed
 * to the sorting functions.  The return value is negative, zero, or positive
 * depending on whether the array element pointed to by item1 is less than,
 * equal to, or greater than that pointed to by item2.
 *
 * This function must be declared as taking two const void * parameters.
 * Inside the function, the parameters are cast to test_item_t pointers.
 */
unsigned int item_value(const void *item)
{
    return ((test_item_t *)item)->data1;
}


int main(void)
{
    dst_t *t;
    test_item_t item_array[N_ITEMS], *i, *i2, duplicate, lookup_item;
    test_item_t *result;
    int j;


    /* Create a dgital search tree. */
    t = dst_alloc(item_value, MAX_BITS);
    
    /* Add some items to it. */    
    printf("Inserting items:\n");
    for(j = 0; j < N_ITEMS; j++) {
	i = &item_array[j];
	i->data1 = rand() % RND_MAX;
	i->data2 = rand() % RND_MAX;
        if((i2 = dst_insert(t, i))) {
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
    if(dst_insert(t, &duplicate)) {
	printf("duplicate detected.\n");
    }
    else {
	printf("failed - duplicate undetected.\n");
	exit(1);
    }
    
    /* Test dst_find() */
    printf("\nTesting dst_find() to find an item...");
    lookup_item.data1 = item_array[N_ITEMS/2].data1;  /* key to lookup. */
    result = dst_find(t, &lookup_item);
    if(result) {
	printf("%s\n", result->data1 == lookup_item.data1
	       ? "successful." : "failed.");
    }
    else {
	printf("failed.\n");
    }

    /* Test dst_find_min() */
    printf("Testing dst_find_min()...");
    result = dst_find_min(t);
    if(result) printf("%d.\n", result->data1); else printf("failed.\n");

    
    /* Test dst_delete(). */
    printf("\nTesting dst_delete()...");
    result = dst_delete(t, &lookup_item);
    if(result) {
	printf("%s\n", result->data1 == lookup_item.data1
	       ? "successful." : "failed.");
    }
    else {
	printf("failed.\n");
    }

    /* Test dst_delete_min() */
    printf("Testing dst_delete_min()...");
    result = dst_delete_min(t);
    if(result) printf("%d.\n", result->data1); else printf("failed.\n");

    /* Test dst_free(). */
    printf("Testing dst_free()...");
    dst_free(t);
    printf("successful.\n");
    
    /* Create a new tree then repeat dst_insert()/dst_delete(). */
    t = dst_alloc(item_value, MAX_BITS);
    printf("\nRepeating dst_insert()/dst_delete()...\n");
    for(j = 0; j < N_ITEMS; j++) {
	i = &item_array[j];  /* Uniqueness of keys was enforced earlier. */
	printf("Inserting key %d:\n", i->data1);
        dst_insert(t, i);
	dst_dump(t->root, 0);
    }
    for(j = N_ITEMS/2; j < N_ITEMS; j++) {
	i = &item_array[j];
	printf("Deleting key %d:\n", i->data1);
	if(!dst_delete(t, i)) { printf("failed.\n"); exit(1); }
	dst_dump(t->root, 0);
    }
    for(j = N_ITEMS/2; j < N_ITEMS; j++) {
	i = &item_array[j];
	printf("Inserting key %d:\n", i->data1);
        dst_insert(t, i);
	dst_dump(t->root, 0);
    }
    for(j = 0; j < N_ITEMS/2; j++) {
	i = &item_array[j];
	printf("Deleting key %d:\n", i->data1);
	if(!dst_delete(t, i)) { printf("failed.\n"); exit(1); }
	dst_dump(t->root, 0);
    }
    for(j = N_ITEMS/2; j < N_ITEMS; j++) {
	i = &item_array[j];
	printf("Deleting key %d:\n", i->data1);
        if(!dst_delete(t, i)) { printf("failed.\n"); exit(1); }
	dst_dump(t->root, 0);
    }
    printf("successful.\n");    

    /* Test delete_min. */
    for(j = 0; j < N_ITEMS; j++) {
	i = &item_array[j];  /* Uniqueness of keys was enforced earlier. */
	printf("Inserting key %d:\n", i->data1);
        dst_insert(t, i);
	dst_dump(t->root,0);
    }
    for(j = 0; j < N_ITEMS; j++) {
        if(!(i = dst_delete_min(t))) { printf("failed.\n"); exit(1); }
	printf("Deleted minimum key %d:\n", i->data1);
	dst_dump(t->root, 0);
    }
    dst_free(t);
    
    return 0;
}
