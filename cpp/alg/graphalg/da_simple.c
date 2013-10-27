/***  File: da_simple.c  ***/
/*
 *    Shane Saunders
 */

/* Implementations of Dijkstra's Algorithm Using Each of the Heaps.
 * Random graphs are generated using dgraph_sparse.h.
 */

#include <stdio.h>
#include <stdlib.h>
#include "../graphs/dgraph.h"
#include "../timing/timing.h"


/* This program generates key comparisons data or CPU time data, depending on
 * which main() function is selected.
 *
 * To select the main() function for generating key comparisons data use:
 *     #define KEY_COMPS_DATA 1
 * To select the main() function for generating CPU time data use:
 *     #define CPU_TIME_DATA 1
 * Use one or the other, but not both at the same time.
 */
#define KEY_COMPS_DATA 0
#define CPU_TIME_DATA 1


/* Use SHOW_d set to 1 to output the array d[]. */
#define SHOW_d 0


/*** Structure type definitions ***/

/* Timing results structure. */
typedef struct time_res {
    clockval_t ticks;
    long key_comps;
} time_res_t;


/*** Function prototypes. ***/

/* Implementations of Dijkstra's algorithm.
 */
time_res_t dijkstra(dgraph_t *g);

/* Debugging functions. */
#if SHOW_a
void dump_array1(int *a, int n);
#endif
#if SHOW_d
void dump_array(int **a, int n);
#endif


/*** Special values. ***/

#define TRUE 1
#define FALSE 0


/*** main() function depends on the program being compiled. ***/

#if KEY_COMPS_DATA
/** Generate data for the number of key comparisons. **/

/* Step size between values of n used. */
#define STEP 20

int main(int argc, char *argv[])
{
    int i, k, n_max, n_samples;
    double edge_f;
    time_res_t r;
    time_res_t sum;
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


    /* Run Dijkstra's algorithm. */
    printf("Number of Comparisons for Dijkstra's algorithm.\n");
    printf("Edge factor = %.3f, Number of samples = %d\n", edge_f, n_samples);
    printf("\nResults (n, time):\n");
    for(k = STEP; k <= n_max; k += STEP) {

	/* Initialize the sums used in average calculations to zero. */
	sum.ticks = 0;
        sum.key_comps = 0;

	/* For the average calculation, sum the number of comparisons. */
	for(i = 0; i < n_samples; i++) {

	    /* We use a new random graph for each run of Dijkstra's algorithm.
	     */
            graph = dgraph_rnd_sparse(k, edge_f);

	    r = dijkstra(graph);
            sum.key_comps += r.key_comps;

	    /* Free the random graph. */
            dgraph_free(graph);
	}

	printf("%d\t%f\n", k, (double)sum.key_comps/n_samples);
    }

    return 0;
}
#endif

#if CPU_TIME_DATA
/** Generate CPU time data. **/

/* Step size between values of n used. */
#define STEP 50

int main(int argc, char *argv[])
{
    int i, k, n_max, n_samples;
    double edge_f;
    time_res_t r;
    time_res_t sum;
    dgraph_t *graph;
    
    
    /* Process command line arguments (if any), otherwise get input from the
     * user.  Arguments supplied correspond to:
     *     - number of samples used for average calculation.
     *     - maximum graph size to test.
     *     - edge factor (i.e. the average size of the out set).
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

	/* The user should ensure the edge factor entered is greater than:
	 *     1 - 1/n
	 */
	printf("Enter the edge factor (i.e. average size of the OUT set): ");
	scanf("%lf", &edge_f);
	putchar('\n');
    }


    /* Run the binary heap, 2-3 heap, and F-heap implementations of Dijkstra's
     * algorithm.
     */
    printf("CPU Time for Dijkstra's algorithm.\n");
    printf("Edge factor = %.3f, Number of samples = %d\n", edge_f, n_samples);

    printf("\nResults, (n, time):\n");
    for(k = STEP; k <= n_max; k += STEP) {
	
	/* Initialize the sums used in average calculations to zero. */
	sum.ticks = 0;
        sum.key_comps = 0;

	/* For the average calculation, sum the number of comparisons. */
	for(i = 0; i < n_samples; i++) {

	    /* We use a new random graph for each run of Dijkstra's algorithm.
	     */
            graph = dgraph_rnd_sparse(k, edge_f);
	    
	    r = dijkstra(graph);
            sum.ticks += r.ticks;

	    /* Free the random graph. */
            dgraph_free(graph);
	}
	
	printf("%d\t%f\n", k,
	       ((double)sum.ticks/n_samples)/CLOCK_DIV);
    }

    return 0;
}
#endif



/* One dimensional array implementation of Dijkstra's algorithm.
 * Requires a pointer, g, to the directed graph used.
 * Returns the time taken by the algorithm.
 */
time_res_t dijkstra(dgraph_t *g)
{
    /* Index variables. */
    int j, q, v, w;

    /* d[] - distance to vertices.
     * front[] - stores the index to vertices that are in the frontier.
     * p[] - position of vertices in front[] array.
     * f[] - boolean frontier set array.
     * s[] - boolean solution set array.
     * 
     * timing - timing results structure for returning the time taken.
     * dist - used in distance computations.
     * vertices, n - Graph details: vertices array, size.
     * out_n - Current vertex's OUT set size.
     */
    int *p, *front, *f, *s;
    long key_comps;
    long *d, dist;
    dgraph_vertex_t *vertices;
    int n;
    size_t int_size, long_size;
    time_res_t timing;
    dgraph_edge_t *edge_ptr;

    
    int_size = sizeof(int);
    long_size = sizeof(long);
    
    vertices = g->vertices;
    n = g->n;
    
    d = calloc(n, long_size);

    front = calloc(n, int_size);
    p = calloc(n, int_size);

    f = calloc(n, int_size);
    s = calloc(n, int_size);


    /* Start of Dijkstra's algorithm. */

    timer_start();  key_comps = 0;

    /* The start vertex is part of the solutions set. */
    s[StartVertex] = TRUE;
    d[StartVertex] = 0;

    /* j is used as the size of the frontier set. */
    j = 0;

    /* Put out set of the starting vertex into the frontier and update the
     * distances to vertices in the out set.  k is the index for the out set.
     */
    edge_ptr = vertices[StartVertex].first_edge;
    while(edge_ptr) {
        w = edge_ptr->vertex_no;
	d[w] = edge_ptr->dist;
        front[j] = w;
	p[w] = j;
        f[w] = TRUE;
        j++;
	edge_ptr = edge_ptr->next;
    }


    /* At this point we are assuming that all vertices are reachable from
     * the starting vertex and N > 1 so that j > 0.
     */

    while(j > 0) {

        /* Find the vertex in frontier that has minimum distance. */
        v = front[0];
        for(q = 1; q < j; q++) {
	    key_comps++;
            if(d[front[q]] < d[v]) v = front[q];
        }

        /* Move this vertex from the frontier to the solution set.  It's old
         * position in the frontier array becomes occupied by the element
         * previously at the end of the frontier.
         */
        j--;
        front[p[v]] = front[j];
        p[front[j]] = p[v];
        s[v] = TRUE;
        f[v] = FALSE;

        /* Values in p[v] and front[j] no longer used, so are now meaningless
         * at this point.
         */

        /* Update distances to vertices, w, in the out set of v.
         */
        edge_ptr = vertices[v].first_edge;
	while(edge_ptr) {
	    w = edge_ptr->vertex_no;

	    /* Only update if w is not already in the solution set.
             */
            if(s[w] == FALSE) {
		
                /* If w is in the frontier the new distance to w is the minimum
                 * of its current distance and the distance to w via v.
                 */
                dist = d[v] + edge_ptr->dist;
                if(f[w] == TRUE) {
		    key_comps++;
                    if(dist < d[w]) d[w] = dist;
                }
                else {
                    d[w] = dist;
                    front[j] = w;
                    p[w] = j;
                    f[w] = TRUE;
                    j++;
                }
            } /* if */
	    
            edge_ptr = edge_ptr->next;
	} /* while */

    } /* while */

    /* End of Dijkstra's algorithm. */
    
    timing.ticks = timer_stop();
    timing.key_comps = key_comps;

    /* Free space taken arrays local to this function. */
    free(p);
    free(f);
    free(s);
    free(front);
    free(d);

    return timing;
}
