#include <stdio.h>
#include <stdlib.h>
#include "avl.h"


#define DEBUG 1

#define N_ITEMS 150
#define RND_MAX 1000


int exit_flag;


typedef struct test_item {
    int data1;
    int data2;
} test_item_t;


/* A function to produce a rough output of the AVL tree. */
#if DEBUG
int avl_dump_r(avl_node_t *p, int l)
{
    int i;
    int ld, rd;
    
    printf("%d (%d)\n", ((test_item_t *)p->item)->data1, p->balance);

    if(!(p->left || p->right)) return 0;

    for(i = 0; i <= l; i++) {
	putchar(' ');
	putchar(' ');
    }    
    if(p->left) {
	ld = 1 + avl_dump_r(p->left, l+1);
    }
    else {
	ld = 0;
	printf("-\n");
    }
    
    for(i = 0; i <= l; i++) {
	putchar(' ');
	putchar(' ');
    }
    if(p->right) {
        rd = 1 + avl_dump_r(p->right, l+1);
    }
    else {
	rd = 0;
	printf("-\n");
    }

    if(p->balance < -1 || p->balance > 1) {
	for(i = 0; i <= l; i++) {
	    putchar(' ');
	    putchar(' ');
	}
	printf("(error)\n");
	exit_flag = 1;
    }
    if(rd - ld != p->balance) {
	for(i = 0; i <= l; i++) {
	    putchar(' ');
	    putchar(' ');
	}
	printf("(error)\n");
	exit_flag = 1;
    }

    
    if(l == 0 && exit_flag == 1) {
	printf("\nerror\n");
	exit(1);
    }

    return ld > rd ? ld : rd;
}
#else
int avl_dump_r(avl_node_t *p, int l)
{
    return 0;
}
#endif

    
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
    avl_t *t;
    test_item_t item_array[N_ITEMS], *i, *i2, duplicate, lookup_item;
    test_item_t *result;
    int j;

    exit_flag = 0;  /* For debugging. */

    /* Create an AVL search tree. */
    t = avl_alloc(item_cmp);
    
    /* Add some items to it. */    
    printf("Inserting items:\n");
    for(j = 0; j < N_ITEMS; j++) {
	i = &item_array[j];
	i->data1 = rand() % RND_MAX;
	i->data2 = rand() % RND_MAX;
        if((i2 = avl_insert(t, i))) {
	    printf("  duplicate %d detected - trying again\n", i2->data1);
	    j--;
	}
	else {
/* 	    printf("  %d", i->data1); */
	    printf("Inserted key %d:\n", i->data1);
	    avl_dump_r(t->root, 0);
	}
    }

    /* Test duplicate detection. */
    printf("\nAttempting insert to give duplicate key...");
    i2 = &item_array[N_ITEMS/2];
    duplicate.data1 = i2->data1;  duplicate.data2 = rand() % RND_MAX;
    if(avl_insert(t, &duplicate)) {
	printf("duplicate detected.\n");
    }
    else {
	printf("failed - duplicate undetected.\n");
	exit(1);
    }
    
    /* Test avl_find() */
    printf("\nTesting avl_find() to find an item...");
    lookup_item.data1 = item_array[N_ITEMS/2].data1;  /* key to lookup. */
    result = avl_find(t, &lookup_item);
    if(result) {
	printf("%s\n", result->data1 == lookup_item.data1
	       ? "successful." : "failed.");
    }
    else {
	printf("failed.\n");
    }

    /* Test avl_find_min() */
    printf("Testing avl_find_min()...");
    result = avl_find_min(t);
    if(result) printf("%d.\n", result->data1); else printf("failed.\n");

    
    /* Test avl_delete(). */
    printf("\nTesting avl_delete()...");
    result = avl_delete(t, &lookup_item);
    if(result) {
	printf("%s\n", result->data1 == lookup_item.data1
	       ? "successful." : "failed.");
    }
    else {
	printf("failed.\n");
    }

    /* Test avl_delete_min() */
    printf("Testing avl_delete_min()...");
    result = avl_delete_min(t);
    if(result) printf("%d.\n", result->data1); else printf("failed.\n");

    /* Test avl_free(). */
    printf("Testing avl_free()...");
    avl_free(t);
    printf("successful.\n");
    
    /* Create a new tree then repeat avl_insert()/avl_delete(). */
    t = avl_alloc(item_cmp);
    printf("\nRepeating avl_insert()/avl_delete()...");
    for(j = 0; j < N_ITEMS; j++) {
	i = &item_array[j];  /* Uniqueness of keys was enforced earlier. */
	printf("Inserting key %d:\n", i->data1);
        avl_insert(t, i);
	avl_dump_r(t->root, 0);
    }
    for(j = N_ITEMS/2; j < N_ITEMS; j++) {
	i = &item_array[j];
	printf("Deleting key %d:\n", i->data1);
	if(!avl_delete(t, i)) { printf("failed.\n"); exit(1); }
	avl_dump_r(t->root, 0);
    }
    for(j = N_ITEMS/2; j < N_ITEMS; j++) {
	i = &item_array[j];
	printf("Inserting key %d:\n", i->data1);
        avl_insert(t, i);
	avl_dump_r(t->root, 0);
    }
    for(j = 0; j < N_ITEMS/2; j++) {
	i = &item_array[j];
	printf("Deleting key %d:\n", i->data1);
	if(!avl_delete(t, i)) { printf("failed.\n"); exit(1); }
	avl_dump_r(t->root, 0);
    }
    for(j = N_ITEMS/2; j < N_ITEMS; j++) {
	i = &item_array[j];
	printf("Deleting key %d:\n", i->data1);
        if(!avl_delete(t, i)) { printf("failed.\n"); exit(1); }
	if(t->root) avl_dump_r(t->root, 0);
    }
    printf("successful.\n");    

    /* Test delete_min. */
    for(j = 0; j < N_ITEMS; j++) {
	i = &item_array[j];  /* Uniqueness of keys was enforced earlier. */
	printf("Inserting key %d:\n", i->data1);
        avl_insert(t, i);
	avl_dump_r(t->root, 0);
    }
    for(j = 0; j < N_ITEMS; j++) {
        if(!(i = avl_delete_min(t))) { printf("failed.\n"); exit(1); }
	printf("Deleted minimum key %d:\n", i->data1);
	if(t->root) avl_dump_r(t->root, 0);
    }
    avl_free(t);
    
    return 0;
}
