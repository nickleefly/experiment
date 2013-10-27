#include <stdio.h>
#include <stdlib.h>
#include "tree23_ext.h"

#define N_ITEMS 8000
#define RND_MAX 20000

int exit_flag;


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


#ifdef DEBUG
/* A function to produce a rough output of the 2-3 tree. */
void *tree23_dump_r(tree23_node_t *p, int l)
{
    void *k, *min_k;

    
    spaces(2*l);printf("[%d]", (int)p);
    if(p->key_node1) {
	printf("(%d)", (int)(p->key_node1));
    }
    else {
	printf("(--)");
    }
    if(p->key_node2) {
	printf("(%d)", (int)(p->key_node2));
    }
    else {
	printf("(--)");
    }
    if(!p->key_node1) {
	if(p->left.item) {
	    printf(" %d", ((test_item_t *)p->left.item)->data1);
	}
	else {
	    printf(" --");
	}
	if(p->middle.item) {
	    printf(" %d", ((test_item_t *)p->middle.item)->data1);
	}
	else {
	    printf(" --");
	}
	if(p->right.item) {
	    printf(" %d", ((test_item_t *)p->right.item)->data1);
        }
	putchar('\n');

        if(p->key_node2) {
	    spaces(2*l);  printf("error - incorrect key\n");
	    exit(1);
	}
	
	return p->left.item;
    }
    else {
	putchar('\n');
	min_k = tree23_dump_r(p->left.node, l+1);
	if(p->left.node->parent != p) {
	    spaces(2*l);  printf("error - child has incorrect parent.\n");
	    exit(1);
	}
	k = tree23_dump_r(p->middle.node, l+1);
	if(p->middle.node->parent != p) {
	    spaces(2*l);  printf("error - child has incorrect parent.\n");
	    exit(1);
	}
	if(p->key_node1->left.item != k) {
	    spaces(2*l);  printf("error - incorrect key1\n");
	    exit(1);
	}
	if(p->key_node2) {
	    k = tree23_dump_r(p->right.node, l+1);
	    if(p->right.node->parent != p) {
		spaces(2*l);  printf("error - child has incorrect parent.\n");
		exit(1);
	    }
	    if(p->key_node2->left.item != k) {
		spaces(2*l);  printf("error - incorrect key2\n");
		exit(1);
	    }
	}
	else {
            if(p->right.node) {
		spaces(2*l);  printf("error - incorrect key2\n");
                exit(1);
	    }
	}

	return min_k;
    }
}
#else
void *tree23_dump_r(tree23_node_t *p, int l)
{
    return NULL;
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
    tree23_t *t;
    test_item_t item_array[N_ITEMS], *i, *i2, duplicate, lookup_item;
    test_item_t *result;
    int j;

    exit_flag = 0;  /* For debugging. */

    /* Create a 2-3 tree. */
    t = tree23_alloc(item_cmp);
    
    /* Add some items to it. */ 
    printf("Inserting items:\n");
    for(j = 0; j < N_ITEMS; j++) {
	i = &item_array[j];
	i->data1 = rand() % RND_MAX;
	i->data2 = rand() % RND_MAX;
        if((i2 = tree23_insert(t, i))) {
	    printf("  duplicate %d detected - trying again\n", i2->data1);
	    j--;
	}
	else {
/* 	    printf("  %d", i->data1); */
	    printf("Inserted key %d:\n", i->data1);
	    tree23_dump_r(t->root, 0);
	}
    }

    /* Test duplicate detection. */
    printf("\nAttempting insert to give duplicate key...");
    i2 = &item_array[N_ITEMS/2];
    duplicate.data1 = i2->data1;  duplicate.data2 = rand() % RND_MAX;
    if(tree23_insert(t, &duplicate)) {
	printf("duplicate detected.\n");
    }
    else {
	printf("failed - duplicate undetected.\n");
	exit(1);
    }
    
    /* Test tree23_find() */
    printf("\nTesting tree23_find() to find an item...");
    lookup_item.data1 = item_array[N_ITEMS/2].data1;  /* key to lookup. */
    result = tree23_find(t, &lookup_item);
    if(result) {
	printf("%s\n", result->data1 == lookup_item.data1
	       ? "successful." : "failed.");
    }
    else {
	printf("failed.\n");
    }

    /* Test tree23_find_min() */
    printf("Testing tree23_find_min()...");
    result = tree23_find_min(t);
    if(result) printf("%d.\n", result->data1); else printf("failed.\n");

    
    /* Test tree23_delete(). */
    printf("\nTesting tree23_delete()...");
    result = tree23_delete(t, &lookup_item);
    if(result) {
	printf("%s\n", result->data1 == lookup_item.data1
	       ? "successful." : "failed.");
    }
    else {
	printf("failed.\n");
    }

    /* Test tree23_delete_min() */
    printf("Testing tree23_delete_min()...");
    result = tree23_delete_min(t);
    if(result) printf("%d.\n", result->data1); else printf("failed.\n");


    /* Test tree23_free(). */
    printf("Testing tree23_free()...");
    tree23_free(t);
    printf("successful.\n");

    
    /* Create a new tree then repeat tree23_insert()/tree23_delete(). */
    t = tree23_alloc(item_cmp);
    printf("\nRepeating tree23_insert()/tree23_delete()...");
    for(j = 0; j < N_ITEMS; j++) {
	i = &item_array[j];  /* Uniqueness of keys was enforced earlier. */
	printf("Inserting key %d:\n", i->data1);
        tree23_insert(t, i);
	tree23_dump_r(t->root, 0);
    }
    for(j = N_ITEMS/2; j < N_ITEMS; j++) {
	i = &item_array[j];
	printf("Deleting key %d:\n", i->data1);
	if(!tree23_delete(t, i)) { printf("failed.\n"); exit(1); }
	tree23_dump_r(t->root, 0);
    }
    for(j = N_ITEMS/2; j < N_ITEMS; j++) {
	i = &item_array[j];
	printf("Inserting key %d:\n", i->data1);
        tree23_insert(t, i);
	tree23_dump_r(t->root, 0);
    }
    for(j = 0; j < N_ITEMS/2; j++) {
	i = &item_array[j];
	printf("Deleting key %d:\n", i->data1);
	if(!tree23_delete(t, i)) { printf("failed.\n"); exit(1); }
	tree23_dump_r(t->root, 0);
    }
    for(j = N_ITEMS/2; j < N_ITEMS; j++) {
	i = &item_array[j];
	printf("Deleting key %d:\n", i->data1);
        if(!tree23_delete(t, i)) { printf("failed.\n"); exit(1); }
	if(t->root) tree23_dump_r(t->root, 0);
    }
    printf("successful.\n");    

    /* Test delete_min. */
    for(j = 0; j < N_ITEMS; j++) {
	i = &item_array[j];  /* Uniqueness of keys was enforced earlier. */
	printf("Inserting key %d:\n", i->data1);
        tree23_insert(t, i);
	tree23_dump_r(t->root, 0);
    }
    for(j = 0; j < N_ITEMS; j++) {
        if(!(i = tree23_delete_min(t))) { printf("failed.\n"); exit(1); }
	printf("Deleted minimum key %d:\n", i->data1);
	if(t->root) tree23_dump_r(t->root, 0);
    }
    tree23_free(t);
    
    return 0;
}
