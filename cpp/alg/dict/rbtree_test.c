#include <stdio.h>
#include <stdlib.h>
#include "rbtree.h"


#define DEBUG 0


#define N_ITEMS 15000
#define RND_MAX 30000


typedef struct test_item {
    int data1;
    int data2;
} test_item_t;


#if DEBUG
/* Funtion for indenting output. */
void spaces(int l)
{
    int i;
    for(i = 0; i < l; i++) {
        putchar(' ');
    }
}

int rbtree_dump(rbtree_node_t *p, int l)
{
    int lsum, rsum;
    rbtree_colour_t rcol, lcol;

    if(!p) return 0;
    
    spaces(2*l);  printf("%d (%s)\n", ((test_item_t *)(p->item))->data1,
			 p->colour == Red ? "Red" : "Black");
    
    if(p->left) {
	lsum = rbtree_dump(p->left, l+1);
	lcol = p->left->colour;
/* 	if(p->left->parent != p) { */
/* 	    spaces(2*(l+1)); printf("Incorrect parent pointer.\n"); */
/* 	    exit(1); */
/* 	} */
    }
    else {
	if(p->right) { spaces(2*(l+1));  printf("--\n"); }
	lsum = 0;
	lcol = Black;
    }
    
    if(p->right) {
	rsum = rbtree_dump(p->right, l+1);
	rcol = p->right->colour;
/* 	if(p->right->parent != p) { */
/* 	    spaces(2*(l+1)); printf("Incorrect parent pointer.\n"); */
/* 	    exit(1); */
/* 	} */
    }
    else {
	if(p->left) { spaces(2*(l+1));  printf("--\n"); }
	rsum = 0;
	rcol = Black;
    }

    if(lsum != rsum) {
	spaces(2*l);
	printf("Ballance of left and right sides is wrong.\n");
	exit(1);
    }

    if(p->colour == Black) {
	lsum++;
    }
    else {
        if(lcol == Red || rcol == Red) {
	    spaces(2*l);  printf("double-red problem\n");
	    exit(1);
	}
    }

    return lsum;
}
#else
int rbtree_dump(rbtree_node_t *p, int l) {
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
    rbtree_t *t;
    test_item_t item_array[N_ITEMS], *i, *i2, duplicate, lookup_item;
    test_item_t *result;
    int j;

    /* Create a red-black tree. */
    t = rbtree_alloc(item_cmp);
    
    /* Add some items to it. */    
    printf("Inserting items:\n");
    for(j = 0; j < N_ITEMS; j++) {
	i = &item_array[j];
	i->data1 = rand() % RND_MAX;
	i->data2 = rand() % RND_MAX;
        if((i2 = rbtree_insert(t, i))) {
	    printf("  duplicate %d detected - trying again\n", i2->data1);
	    j--;
	}
	else {
	    printf("inserting %d", i->data1);
putchar('\n');  rbtree_dump(t->root, 0);
	}
    }

    /* Test duplicate detection. */
    printf("\nAttempting insert to give duplicate key...");
    i2 = &item_array[N_ITEMS/2];
    duplicate.data1 = i2->data1;  duplicate.data2 = rand() % RND_MAX;
    if(rbtree_insert(t, &duplicate)) {
	printf("duplicate detected.\n");
    }
    else {
	printf("failed - duplicated undected.\n");
	exit(1);
    }
    
    /* Test rbtree_find() */
    printf("\nTesting rbtree_find() to find an item...");
    lookup_item.data1 = item_array[N_ITEMS/2].data1;  /* key to lookup. */
    result = rbtree_find(t, &lookup_item);
    if(result) {
	printf("%s\n", result->data1 == lookup_item.data1
	       ? "successful." : "failed.");
    }
    else {
	printf("failed.\n");
    }

    /* Test rbtree_find_min() */
    printf("Testing rbtree_find_min()...");
    result = rbtree_find_min(t);
    if(result) printf("%d.\n", result->data1); else printf("failed.\n");

    
    /* Test rbtree_delete(). */
    printf("\nTesting rbtree_delete()...");
    result = rbtree_delete(t, &lookup_item);
    if(result) {
	printf("%s\n", result->data1 == lookup_item.data1
	       ? "successful." : "failed.");
    }
    else {
	printf("failed.\n");
    }

    /* Test rbtree_delete_min() */
    printf("Testing rbtree_delete_min()...");
    result = rbtree_delete_min(t);
    if(result) printf("%d.\n", result->data1); else printf("failed.\n");

    /* Test rbtree_free(). */
    printf("Testing rbtree_free()...");
    rbtree_free(t);
    printf("successful.\n");
    
    /* Create a new tree then repeat rbtree_insert()/rbtree_delete(). */
    t = rbtree_alloc(item_cmp);
    printf("\nRepeating rbtree_insert()/rbtree_delete()...");
    for(j = 0; j < N_ITEMS; j++) {
	i = &item_array[j];  /* Uniqueness of keys was enforced earlier. */
        rbtree_insert(t, i);
printf("inserted %d\n", i->data1);
rbtree_dump(t->root, 0);
    }
    for(j = N_ITEMS/2; j < N_ITEMS; j++) {
	i = &item_array[j];
	if(!rbtree_delete(t, i)) { printf("failed.\n"); exit(1); }
printf("deleted %d\n", i->data1);
rbtree_dump(t->root, 0);
    }
    for(j = N_ITEMS/2; j < N_ITEMS; j++) {
	i = &item_array[j];
        rbtree_insert(t, i);
printf("inserted %d\n", i->data1);
rbtree_dump(t->root, 0);
    }
    for(j = 0; j < N_ITEMS/2; j++) {
	i = &item_array[j];
	if(!rbtree_delete(t, i)) { printf("failed.\n"); exit(1); }
printf("deleted %d\n", i->data1);
rbtree_dump(t->root, 0);
    }
    for(j = N_ITEMS/2; j < N_ITEMS; j++) {
	i = &item_array[j];
        if(!rbtree_delete(t, i)) { printf("failed.\n"); exit(1); }
printf("deleted %d\n", i->data1);
rbtree_dump(t->root, 0);
    }
    printf("successful.\n");

    rbtree_free(t);
    
    return 0;
}
