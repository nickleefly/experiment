/*** File:  sort_test.c  - Example Program Using sort.c and sort.h ***/
/*
 *   Shane Saunders
 */
#include <stdio.h>
#include <stdlib.h>
#include "../timing/timing.h"
#include "sort.h"


/* Number of items in the array to be sorted. */
#define N_ITEMS 15

/* Maximum value of array items for sorting time test. */
#define MAX_VALUE 10000

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


/* get_value() - returns an integer representing the value of an array item.
 */
int get_value(const void *item)
{
    return ((test_item_t *)item)->key;
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
    int i, j, n_samples, max_n, step_n;
    int array_size;
    int radix;
    test_item_t a[N_ITEMS], test_array[N_ITEMS];
    test_item_t *timing_array, *copy_array;
    timing_t *t;
    
    /* Assign random values to the key of each array element. */
    rand_array(a, N_ITEMS, 100);

    /* Now test quicksort(). */
    memcpy(test_array, a, sizeof(test_array));
    printf("array before quicksort\n = ");
    print_array(test_array, N_ITEMS);
    printf("\n\n");
    quicksort(test_array, N_ITEMS, sizeof(test_item_t), item_cmp);
    printf("array after quicksort\n = ");
    print_array(test_array, N_ITEMS);
    printf("\n\n");

    /* Now test mergesort0(). */
    memcpy(test_array, a, sizeof(test_array));
    printf("array before mergesort0\n = ");
    print_array(test_array, N_ITEMS);
    printf("\n\n");
    mergesort0(test_array, N_ITEMS, sizeof(test_item_t), item_cmp);
    printf("array after mergesort0\n = ");
    print_array(test_array, N_ITEMS);
    printf("\n\n");

    /* Now test mergesort(). */
    memcpy(test_array, a, sizeof(test_array));
    printf("array before mergesort\n = ");
    print_array(test_array, N_ITEMS);
    printf("\n\n");
    mergesort(test_array, N_ITEMS, sizeof(test_item_t), item_cmp);
    printf("array after mergesort\n = ");
    print_array(test_array, N_ITEMS);
    printf("\n\n");
    
    /* Now test radix sort. */
    memcpy(test_array, a, sizeof(test_array));
    printf("array before radixsort\n = ");
    print_array(test_array, N_ITEMS);
    printf("\n\n");
    radixsort(test_array, N_ITEMS, sizeof(test_item_t), get_value, 10);
    printf("array after radixsort\n = ");
    print_array(test_array, N_ITEMS);
    printf("\n\n");

    /* Now test heapsort. */
    memcpy(test_array, a, sizeof(test_array));
    printf("array before heapsort\n = ");
    print_array(test_array, N_ITEMS);
    printf("\n\n");
    heapsort(test_array, N_ITEMS, sizeof(test_item_t), item_cmp);
    printf("array after heapsort\n = ");
    print_array(test_array, N_ITEMS);
    printf("\n\n");
    
    /* Time the quicksort and mergesort sorting functions. */

    printf("Enter the number of samples to use: ");
    scanf("%d", &n_samples);
    printf("Enter the maximum array length to sort: ");
    scanf("%d", &max_n);
    printf("Enter the step size for array lengths: ");
    scanf("%d", &step_n);

    t = timing_alloc(5);  /* Five different sorting algorithms. */
    
    printf("\nResults (n, qsort, quicksort, mergesort, mergesort0, heapsort) (msec)\n"
	  );
    for(i = step_n; i <= max_n; i += step_n) {
	array_size = i * sizeof(test_item_t);
        timing_array = malloc(array_size);
	copy_array = malloc(array_size);
	rand_array(copy_array, i, MAX_VALUE);

        timing_reset(t);
	
	for(j = 0; j < n_samples; j++) {
	    memcpy(timing_array, copy_array, array_size);
            timing_start();
	    qsort(timing_array, i, sizeof(test_item_t), item_cmp);
            timing_stop(t,0);

	    memcpy(timing_array, copy_array, array_size);
            timing_start();
	    quicksort(timing_array, i, sizeof(test_item_t), item_cmp);
            timing_stop(t,1);

	    memcpy(timing_array, copy_array, array_size);
            timing_start();
	    mergesort(timing_array, i, sizeof(test_item_t), item_cmp);
            timing_stop(t,2);

	    memcpy(timing_array, copy_array, array_size);
            timing_start();
	    mergesort0(timing_array, i, sizeof(test_item_t), item_cmp);
            timing_stop(t,3);

	    memcpy(timing_array, copy_array, array_size);
            timing_start();
	    heapsort(timing_array, i, sizeof(test_item_t), item_cmp);
            timing_stop(t,4);
	}
	printf("%d", i);
	timing_print(t,"\t%.2f",n_samples);
	putchar('\n');

	free(timing_array);
	free(copy_array);
    }
    timing_free(t);

    /* Time radix sort on the largest array, using different radix sizes. */
    printf("\nRadix Sort Results.  Using n = %d\n", max_n);
    printf("(radix, time)\n");
    array_size = max_n * sizeof(test_item_t);
    timing_array = malloc(array_size);
    copy_array = malloc(array_size);
    rand_array(copy_array, max_n, MAX_VALUE);
    for(radix = 2; radix <= max_n; radix <<= 1) {

        timer_reset();
	
	for(j = 0; j < n_samples; j++) {
	    memcpy(timing_array, copy_array, array_size);
            timer_start();
	    radixsort(timing_array, max_n, sizeof(test_item_t), get_value,
		      radix);
            timer_stop();
	}
	
	printf("%d", radix);
	timer_print("\t%.2f", n_samples);
	putchar('\n');
    }
    free(timing_array);
    free(copy_array);
	
    return 0;
}
