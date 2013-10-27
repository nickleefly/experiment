/*** mst_test.c - Test minimal spanning tree algorithms. ***/
/*
 *   Shane Saunders
 */
#include <stdio.h>
#include <stdlib.h>
#include "mst.h"
#include "../graphs/dgraph.h"
#include "../timing/timing.h"

/* #include all heaps to be tested using Prim's algorithm. */
#include "../heaps/bheap.h"
#include "../heaps/fheap.h"
#include "../heaps/ttheap.h"
#include "../heaps/triheap.h"
#include "../heaps/triheap_ext.h"


/* Step size between values of n used. */
#define STEP 200

/* Structure type describing each heap. */
typedef struct heapdesc {
    char *desc;  /* Heap description (i.e. name) */
    const heap_info_t *fns;  /* Heap functions (passed to algorithm). */
} heapdesc_t;

/* An array of time info structures holds the information for each dictionary.
 */
heapdesc_t heaps[] = {
    { "Binary",&BHEAP_info },
    { "Fibonacci",&FHEAP_info },
    { "2-3",&TTHEAP_info },
    { "Trinomial",&TRIHEAP_info },
    { "Trinomial (ext)",&TRIHEAP_EXT_info },
};


/*** Main Program ***/
int main(int argc, char *argv[])
{
    int i, j, k, n_max, n_samples, n_heaps;
    double edge_f;
    mst_result_t *r;
    dgraph_t *graph;
    timing_t *t;

    /* Process command line arguments (if any), otherwise get input from the
     * user.  Arguments supplied correspond to:
     *     - number of samples used for average calculation.
     *     - maximum graph size to test.
     *     - edge factor (the average size of the out set).
     */
    if(argc == 4) {
        n_samples = atoi(argv[1]);
        n_max = atoi(argv[2]);
        edge_f = atof(argv[3]);
    }
    else {

	/* Several samples may be needed to calculate the time to the required
	 * accuracy.
	 */
	printf("Enter the number samples to use: ");
	scanf("%d", &n_samples);

        /* We have an upper limit on the graph size tested. */
	printf("Enter the maximum value of n to use: ");
	scanf("%d", &n_max);

	/* The edge factor entered should be greater than 1 - 1/n
	 */
	printf("Enter the edge factor (i.e. average size of OUT set): ");
	scanf("%lf", &edge_f);
	putchar('\n');
    }

    /* Number of heaps being tested. */
    n_heaps = sizeof(heaps)/sizeof(heapdesc_t);

    /* Allocate structure for timing information. */
    t = timing_alloc(n_heaps);

    printf("CPU Time for Prim's Algorithm (msec)\n");    
    printf("Graph Size = n, Edge factor = %.3f, Number of samples = %d\n", edge_f, n_samples);

    /* Print collumn labels */
    printf("\nResults:\nn");
    for(j = 0; j < n_heaps; j++) printf(",\t%s", heaps[j].desc);
    putchar('\n');
    
    /* Test over varying graph sizes. */
    for(k = STEP; k <= n_max; k += STEP) {

	/* Initialize the sums used in average calculations to zero. */
        timing_reset(t);

	/* For the average calculation, sum the number of comparisons. */
	for(i = 0; i < n_samples; i++) {

	    /* We use a new random graph for each run of Prim's algorithm.
	     */
            graph = dgraph_rnd_sparse(k, edge_f);

	    /* Time each heap. */
	    /* The heap_info struture of each heap is passed to Prim's
	     * algorithm in order to use the functions provided by that heap.
	     */
	    for(j = 0; j < n_heaps; j++) {
		timing_start();
	        r = mst_prim(graph, StartVertex, heaps[j].fns);
                timing_stop(t,j);
	        mst_result_free(r);
	    }
	    
	    /* Free the random graph. */
            dgraph_free(graph);
	}

	/* Print line of key comparison results for current value of k. */
	printf("%d", k);  timing_print(t,"\t%.2f",n_samples);  putchar('\n');
    }

    return 0;
}
