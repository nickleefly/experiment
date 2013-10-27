/*** File:  sort_test.c  - Example Program Using sort.c and sort.h ***/
/*
 *   Shane Saunders
 */
#include <stdio.h>
#include <stdlib.h>
#include "../timing/timing.h"
#include "search.h"


#define RAND_SEED 112233


/* For the test, use structure type array elements. */
typedef struct test_item {
    int key;
    int other_data;
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
    return ((test_item_t *)item1)->key - ((test_item_t *)item2)->key;
}


/* print_array() - a simple function to print the contents of the test array.
 */
void print_array(test_item_t *array, int n)
{
    int i;

    for(i = 0; i < n; i++) {
        printf(" %2d", array[i].key);
    }
}


/* rand_array() - assigns random values to array elements. */
void rand_array(test_item_t *array, int n, int max_val)
{
    int i;
    
    for(i = 0; i < n; i++) {
        array[i].key = rand() % max_val;
    }
}


/* Main program. */
int main(void)
{
    int i, j, n, n_samples, n_bytes;
    test_item_t *r1, *r2;
    test_item_t *a, *sorted;
    test_item_t x;
    timing_t *t;

    /* Get the array size and number of samples from the user. */
    printf("Enter the number of samples: ");
    scanf("%d", &n_samples);
    printf("Enter array size: ");
    scanf("%d", &n);
    putchar('\n');
    
    /* Allocate space for arrays. */
    n_bytes = n * sizeof(test_item_t);
    a = malloc(n_bytes);
    sorted = malloc(n_bytes);
    
    /* Assign random values to the key of each array element. */
    rand_array(a, n, 1000000);
    memcpy(sorted, a, n_bytes);
    
    /* Search for key value of 1000000 (won't be found). */
    x.key = 1000000;
    
    /* Use qsort() to sort the array. */
    printf("Sorting array...\n");
    qsort(sorted, n, sizeof(test_item_t), item_cmp);
    printf("\n");

    /* Now try using binary search to locate key. */
    printf("Searching array for key %d...\n", x.key);
    if(binarysearch(&x, sorted, n, sizeof(test_item_t), item_cmp)) {
	printf("\t- found.\n");
    }
    else {
	printf("\t- not found.\n");
    }
    
    /* As a test which will succeed, try locating each array element from the
     * unsorted array, a, in the sorted array.  Time with the C libray function
     * bsearch;
     */
    printf("\nTiming searching functions...\n");
    t = timing_alloc(2);
    for(i = 0; i < n; i++) {
        for(j = 0; j < n_samples; j++) {
            timing_start();
	    r1 = bsearch(&a[i], sorted, n, sizeof(test_item_t), item_cmp);
            timing_stop(t,0);
            if(!r1) {
		printf("Error-null pointer r1.\n");
		exit(1);
	    }
	    
            timing_start();
	    r2 = binarysearch(&a[i], sorted, n, sizeof(test_item_t), item_cmp);
            timing_stop(t,1);
            if(!r2) {
		printf("Error-null pointer r2.\n");
		exit(1);
	    }
	    if(r2 != r1) printf("Items differ (r1 != r2)\n");
        }
    }
    printf("Total time to search for all array items (msec):\n");
    printf("bsearch = %.2f\nbinarysearch = %.2f\n",
	   (((double)timing_total(t,0)/n_samples)/CLOCK_DIV)*1000,
	   (((double)timing_total(t,1)/n_samples)/CLOCK_DIV)*1000);
    timing_free(t);

    free(a);
    free(sorted);
	
    return 0;
}
