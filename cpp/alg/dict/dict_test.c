#include <stdio.h>
#include <stdlib.h>
#include "../timing/timing.h"
#include "bst.h"
#include "avl.h"
#include "tree23.h"
#include "rbtree.h"
#include "dst.h"
#include "rst.h"
#include "rand.h"

#define N_ITEMS 250000

int exit_flag;


typedef struct time_info {
    char *desc;
    const dict_info_t *fns;
    clockval_t ins_time, del_time, dm_time, find_time;
} time_info_t;


/* Items to be indexed.  In this example, items have two fields.  The field
 * data1 is used as the key for indexing purposes (see the item_cmp() function
 * below.  The field data2 is only included for example, to illustrate that
 * items can have several fields.  However, it is used anyway, when obtaining a
 * shuffled oder of data items.
 */
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

/* A second comparison function which is used when shuffling the order of items
 * according to a random value assigned to the data2 field.
 */
int item_cmp2(const void *item1, const void *item2)
{
    return ((test_item_t *)item1)->data2 - ((test_item_t *)item2)->data2;
}

unsigned int item_val(const void *item)
{
    return ((test_item_t *)item)->data1;
}


/* An array of time info structures holds the information for each dictionary.
 */
time_info_t dict_times[] = {
    { "bst",&BST_info,0,0,0,0 },
    { "avl",&AVL_info,0,0,0,0 },
    { "23",&TREE23_info,0,0,0,0 },
    { "RB",&RBTREE_info,0,0,0,0 },
    { "dst",&DST_info,0,0,0,0 },
    { "rst",&RST_info,0,0,0,0 },
};


int main(void)
{
    tree23_t *t23;
    time_info_t *time_struct;
    void *t;  /* pointer to the dictionary */
    test_item_t **item_array, **item_array2, *item;
    int i, j, test_n;


    /* Set up an array of items, using a 2-3 tree to avoid duplicating items.
     */
    item_array = malloc(N_ITEMS * sizeof(test_item_t *));
    t23 = tree23_alloc(item_cmp);    
    printf("Setting up items...\n");
    for(j = 0; j < N_ITEMS; j++) {
	item = item_array[j] = malloc(sizeof(test_item_t));
	item->data1 = rand_generate();
        while(tree23_insert(t23, item)) {  /* duplicated etected */
	    item->data1 = rand_generate();
	}
    }
    tree23_free(t23);
    
    /* Set up a second item array which points to the same items as the first
     * array, but in a different (random) order.
     */
    item_array2 = malloc(N_ITEMS * sizeof(test_item_t *));
    t23 = tree23_alloc(item_cmp2);
    for(j = 0; j < N_ITEMS; j++) {
	item = item_array[j];
	item->data2 = rand_generate();
        while(tree23_insert(t23, item)) {  /* duplicated etected */
	    item->data2 = rand_generate();
	}
    }
    for(j = 0; j < N_ITEMS; j++) {
        item_array2[j] = tree23_delete_min(t23);
    }
    tree23_free(t23);    


    
    /* Create a dictionary and time insert(), delete(), and delete_min(). */
    test_n = sizeof(dict_times)/sizeof(time_info_t);
    for(i = 0; i < test_n; i++) {
	
        time_struct = &dict_times[i];
	
        t = time_struct->fns->alloc(item_cmp,item_val);

	printf("Timing %s:\n", time_struct->desc);

        /* First test null cases for find() and delete(). */
/* 	item = item_array2[0]; */
/* 	if(time_struct->fns->find(t, item)) { */
/* 	    printf("find failed on empty tree\n"); */
/* 	    exit(1); */
/* 	} */
/* 	if(time_struct->fns->find_min(t)) { */
/* 	    printf("find-min failed on empty tree\n"); */
/* 	    exit(1); */
/* 	} */
/* 	if(time_struct->fns->delete(t, item)) { */
/* 	    printf("delete failed on empty tree\n"); */
/* 	    exit(1); */
/* 	} */
/* 	if(time_struct->fns->delete_min(t)) { */
/* 	    printf("delete-min failed on empty tree\n"); */
/* 	    exit(1); */
/* 	} */
	
	printf("  Timing insert()\n");
        timer_start();
        for(j = 0; j < N_ITEMS; j++) {
	    item = item_array[j];
                /* Uniqueness of keys was enforced earlier. */
            time_struct->fns->insert(t, item);
        }
        time_struct->ins_time += timer_stop();

	printf("  Timing find()\n");
	timer_start();
        for(j = 0; j < N_ITEMS; j++) {
	    item = item_array2[j];
	    if(!time_struct->fns->find(t, item)) {
		printf("failed.\n"); exit(1);
	    }
	}
	time_struct->find_time += timer_stop();
	
	printf("  Timing delete()\n");
        timer_start();
        for(j = 0; j < N_ITEMS; j++) {
	    item = item_array2[j];
	    if(!time_struct->fns->delete(t, item)) {
		printf("failed.\n"); exit(1);
	    }
	}
	time_struct->del_time += timer_stop();


	printf("  Timing delete-min()\n");
        for(j = 0; j < N_ITEMS; j++) {
	    item = item_array[j];
                /* Uniqueness of keys was enforced earlier. */
            time_struct->fns->insert(t, item);
        }
        timer_start();
        for(j = 0; j < N_ITEMS; j++) {
	    item = item_array2[j];
	    if(!time_struct->fns->delete_min(t)) {
		printf("failed.\n"); exit(1);
	    }
	}
	time_struct->dm_time += timer_stop();
    }

    free(item_array);
    free(item_array2);

    
    /*** Display Results ***/
    printf("\nResults for %d items:\n", N_ITEMS);
    for(i = 0; i < test_n; i++) {
        printf("\t%s", dict_times[i].desc);
    }
    printf("\nfind");
    for(i = 0; i < test_n; i++) {
	printf("\t%.2f",
	    (double)dict_times[i].find_time/((double)CLOCK_DIV));
    }
    printf("\ninsert");
    for(i = 0; i < test_n; i++) {
	printf("\t%.2f",
	    (double)dict_times[i].ins_time/((double)CLOCK_DIV));
    }
    printf("\ndelete");
    for(i = 0; i < test_n; i++) {
	printf("\t%.2f",
	    (double)dict_times[i].del_time/((double)CLOCK_DIV));
    }
    printf("\ndel-min");
    for(i = 0; i < test_n; i++) {
	printf("\t%.2f",
	    (double)dict_times[i].dm_time/((double)CLOCK_DIV));
    }
    printf("\n");

    
    return 0;
}
