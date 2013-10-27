#include <stdio.h>
#include <stdlib.h>
#include "da.h"
#include "../graphs/dgraph.h"

/* #include all heaps to be tested using dijkstra's algorithm. */
#include "../heaps/bheap.h"
#include "../heaps/fheap.h"
#include "../heaps/ttheap.h"
#include "../heaps/triheap.h"
#include "../heaps/triheap_ext.h"


/* This program generates key comparisons data and/or CPU time data, by
 * selecting on of the following with a 1.
 */
#define KEY_COMPS_DATA 0
#define CPU_TIME_DATA 1

/* Step size between values of n used. */
#define STEP 200

/* Structure type for summing the results for each heap. */
typedef struct timestruct {
    char *desc;  /* Heap description (i.e. name) */
    const heap_info_t *fns;  /* Heap functions (passed to algorithm). */
    da_result_t sum;  /* For summing results of algorithm. */
} timestruct_t;

/* An array of time info structures holds the information for each dictionary.
 */
timestruct_t heap_times[] = {
    { "Binary",&BHEAP_info },
    { "Fibonacci",&FHEAP_info },
    { "2-3",&TTHEAP_info },
    { "Trinomial",&TRIHEAP_info },
    { "Trinomial (ext)",&TRIHEAP_EXT_info },
};

int main(int argc, char *argv[])
{
    int i, j, k, n_max, n_samples, n_heaps;
    double edge_f;
    da_result_t *r;
    dgraph_t *graph;

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
    n_heaps = sizeof(heap_times)/sizeof(timestruct_t);

#if KEY_COMPS_DATA
    printf("Number of Comparisons for Dijkstra's Algorithm.\n");
#endif
#if CPU_TIME_DATA
    printf("CPU Time for Dijkstra's Algorithm (msec)\n");
#endif
    
    printf("Graph Size = n, Edge factor = %.3f, Number of samples = %d\n", edge_f, n_samples);

    /* Print collumn labels */
    printf("\nResults:\nn");
    for(j = 0; j < n_heaps; j++) printf(",\t%s", heap_times[j].desc);
    putchar('\n');
    
    /* Test over varying graph sizes. */
    for(k = STEP; k <= n_max; k += STEP) {

	/* Initialize the sums used in average calculations to zero. */
        for(j = 0; j < n_heaps; j++) {
	    heap_times[j].sum.key_comps = 0;
	    heap_times[j].sum.ticks = 0;
	}

	/* For the average calculation, sum the number of comparisons. */
	for(i = 0; i < n_samples; i++) {

	    /* We use a new random graph for each run of Dijkstra's algorithm.
	     */
            graph = dgraph_rnd_sparse(k, edge_f);

	    /* Time each heap. */
	    /* The heap_info struture of each heap is passed to dijkstra's
	     * algorithm in order to use the functions provided by that heap.
	     */
	    for(j = 0; j < n_heaps; j++) {
	        r = heap_dijkstra(graph, StartVertex, heap_times[j].fns);
                heap_times[j].sum.key_comps += r->key_comps;
                heap_times[j].sum.ticks += r->ticks;
	        da_result_free(r);
	    }
	    
	    /* Free the random graph. */
            dgraph_free(graph);
	}
#if KEY_COMPS_DATA
	/* Print line of key comparison results for current value of k. */
	printf("%d", k);
	for(j = 0; j < n_heaps; j++)
	    printf("\t%.2f", (double)heap_times[j].sum.key_comps/n_samples);
	putchar('\n');
#endif
#if CPU_TIME_DATA
	/* Print line of key comparison results for current value of k. */
	printf("%d", k);
	for(j = 0; j < n_heaps; j++) printf("\t%.2f",
	    (((double)heap_times[j].sum.ticks/n_samples)/CLOCK_DIV)*1000);
	putchar('\n');
#endif
    }

    return 0;
}
