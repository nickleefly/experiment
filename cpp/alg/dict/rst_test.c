#include <stdio.h>
#include <stdlib.h>
#include "rst.h"


#define N_ITEMS 12
#define RND_MAX 64
#define MAX_BITS 6




typedef struct test_item {
    int data1;
    int data2;
} test_item_t;


void indent(int l) {
    int i;
    
    for(i = 0; i < 2*l; i++) {
        putchar(' ');
    }
}


void rst_dump(rst_node_t *p, int l) {
    printf("(%d%d)\n", p->child_links & 1, (p->child_links >> 1)  & 1);
    if(!(p->a[0] || p->a[1])) return;

    
    indent(l+1);
    if(p->a[0]) {
        if(p->child_links & 1) rst_dump(p->a[0], l+1);
	else printf("%d", ((test_item_t *)p->a[0])->data1);
    }
    else {
        printf("--");
    }
    putchar('\n');

    indent(l+1);
    if(p->a[1]) {
        if(p->child_links & 2) rst_dump(p->a[1], l+1);
	else printf("%d", ((test_item_t *)p->a[1])->data1);
    }
    else {
        printf("--");
    }
    putchar('\n');
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
    rst_t *t;
    test_item_t item_array[N_ITEMS], *i, *i2, duplicate, lookup_item;
    test_item_t *result;
    int j;


    /* Create a dgital search tree. */
    t = rst_alloc(item_value, MAX_BITS);
    
    /* Add some items to it. */    
    printf("Inserting items:\n");
    for(j = 0; j < N_ITEMS; j++) {
	i = &item_array[j];
	i->data1 = rand() % RND_MAX;
	i->data2 = rand() % RND_MAX;
        if((i2 = rst_insert(t, i))) {
	    printf("  duplicate %d detected - trying again\n", i2->data1);
	    j--;
	}
	else {
	    printf("Inserted key %d:\n", i->data1);
	    rst_dump(t->root, 0);
	}
    }

    /* Test duplicate detection. */
    printf("\nAttempting insert to give duplicate key...");
    i2 = &item_array[N_ITEMS/2];
    duplicate.data1 = i2->data1;  duplicate.data2 = rand() % RND_MAX;
    if(rst_insert(t, &duplicate)) {
	printf("duplicate detected.\n");
    }
    else {
	printf("failed - duplicate undetected.\n");
	exit(1);
    }
    
    /* Test rst_find() */
    printf("\nTesting rst_find() to find an item...");
    lookup_item.data1 = item_array[N_ITEMS/2].data1;  /* key to lookup. */
    result = rst_find(t, &lookup_item);
    if(result) {
	printf("%s\n", result->data1 == lookup_item.data1
	       ? "successful." : "failed.");
    }
    else {
	printf("failed.\n");
    }

    /* Test rst_find_min() */
    printf("Testing rst_find_min()...");
    result = rst_find_min(t);
    if(result) printf("%d.\n", result->data1); else printf("failed.\n");

    
    /* Test rst_delete(). */
    printf("\nTesting rst_delete()...");
    result = rst_delete(t, &lookup_item);
    if(result) {
	printf("%s\n", result->data1 == lookup_item.data1
	       ? "successful." : "failed.");
    }
    else {
	printf("failed.\n");
    }

    /* Test rst_delete_min() */
    printf("Testing rst_delete_min()...");
    result = rst_delete_min(t);
    if(result) printf("%d.\n", result->data1); else printf("failed.\n");

    /* Test rst_free(). */
    printf("Testing rst_free()...");
    rst_free(t);
    printf("successful.\n");
    
    /* Create a new tree then repeat rst_insert()/rst_delete(). */
    t = rst_alloc(item_value, MAX_BITS);
    printf("\nRepeating rst_insert()/rst_delete()...\n");
    for(j = 0; j < N_ITEMS; j++) {
	i = &item_array[j];  /* Uniqueness of keys was enforced earlier. */
	printf("Inserting key %d:\n", i->data1);
        rst_insert(t, i);
	rst_dump(t->root, 0);
    }
    for(j = N_ITEMS/2; j < N_ITEMS; j++) {
	i = &item_array[j];
	printf("Deleting key %d:\n", i->data1);
	if(!rst_delete(t, i)) { printf("failed.\n"); exit(1); }
	rst_dump(t->root, 0);
    }
    for(j = N_ITEMS/2; j < N_ITEMS; j++) {
	i = &item_array[j];
	printf("Inserting key %d:\n", i->data1);
        rst_insert(t, i);
	rst_dump(t->root, 0);
    }
    for(j = 0; j < N_ITEMS/2; j++) {
	i = &item_array[j];
	printf("Deleting key %d:\n", i->data1);
	if(!rst_delete(t, i)) { printf("failed.\n"); exit(1); }
	rst_dump(t->root, 0);
    }
    for(j = N_ITEMS/2; j < N_ITEMS; j++) {
	i = &item_array[j];
	printf("Deleting key %d:\n", i->data1);
        if(!rst_delete(t, i)) { printf("failed.\n"); exit(1); }
	rst_dump(t->root, 0);
    }
    printf("successful.\n");    

    /* Test delete_min. */
    for(j = 0; j < N_ITEMS; j++) {
	i = &item_array[j];  /* Uniqueness of keys was enforced earlier. */
	printf("Inserting key %d:\n", i->data1);
        rst_insert(t, i);
	rst_dump(t->root,0);
    }
    for(j = 0; j < N_ITEMS; j++) {
        if(!(i = rst_delete_min(t))) { printf("failed.\n"); exit(1); }
	printf("Deleted minimum key %d:\n", i->data1);
	rst_dump(t->root, 0);
    }
    rst_free(t);
    
    return 0;
}
