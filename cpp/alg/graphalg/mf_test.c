#include <stdio.h>
#include "../timing/timing.h"
#include "mf.h"

/*** mf_test.c - Test maximum flow algorithms ***/

/*** --- Settings used for timing the algorithms. ------------------------ ***/

#define N_GRAPHS 3
#define N_SAMPLES 10

/* Two different main() functions can be chosen from. */

/* To time the functions using varying network sizes use:
 *     #define VARY_GRAPH_SIZE 1
 * To time the functions using a varying number of edges in the network use:
 *     #define VARY_GRAPH_SIZE 0
 */
#define VARY_GRAPH_SIZE 0

#if VARY_GRAPH_SIZE

#define EDGE_FACTOR 2
#define STEP_VERTICES 2000
#define MAX_VERTICES 10000

#else

#define N_VERTICES 100
#define STEP_EDGES 3000
#define MAX_EDGES 9000

#endif

/* Specify the range for edge capacities. */
#define MIN_CAPACITY 1
#define MAX_CAPACITY 1000


/*** --- Main Program ---------------------------------------------------- ***/

/* Choose which main() function will be used. */
#if VARY_GRAPH_SIZE
int main(void)
{
    network_t *g;
    timing_t *t;
    int i, j;
    int n;
    int flow;

    /* This computes the average time taken by the maximum flow algorithms over
     * several different random graphs of the same size.  This uses several
     * samples per random graph in order to improve the accuracy.  
     */
    t = timing_alloc(4);
    printf("Timing algorithms...");
    printf("\nCPU Time Results (msec):\n");
    printf("n\tFF\tDinic\tMPM\tKarzanov\n");
    for(n = STEP_VERTICES; n <= MAX_VERTICES; n += STEP_VERTICES) {

        timing_reset(t);

	for(i = 0; i < N_GRAPHS; i++) {
	    g = network_rand(n, EDGE_FACTOR*n, MIN_CAPACITY, MAX_CAPACITY);

	    mf_ford(g);
	    flow = network_flow(g);
	    network_reset(g);
	    
	    /* Time Ford Fulkerson. */
            timing_start();
	    for(j = 0; j < N_SAMPLES; j++) {
		mf_ford(g);
		if(network_flow(g) != flow) {
		    printf("Error in mf_ford result.\n");
		    printf("%d != %d\n", network_flow(g), flow);
		    exit(1);
		}
		network_reset(g);
	    }
            timing_stop(t,0);

	    /* Time Dinic */
            timing_start();
	    for(j = 0; j < N_SAMPLES; j++) {
		mf_dinic(g);
		if(network_flow(g) != flow) {
		    printf("Error in mf_dinic result.\n");
		    printf("%d != %d\n", network_flow(g), flow);
		    exit(1);
		}
		network_reset(g);
	    }
            timing_stop(t,1);

	    /* Time MPM */
            timing_start();
	    for(j = 0; j < N_SAMPLES; j++) {
		mf_mpm(g);
		if(network_flow(g) != flow) {
		    printf("Error in mf_mpm result.\n");
		    printf("%d != %d\n", network_flow(g), flow);
		    exit(1);
		}
		network_reset(g);
	    }
            timing_stop(t,2);

	    /* Time Karzanov */
            timing_start();
	    for(j = 0; j < N_SAMPLES; j++) {
		mf_karzanov(g);
		if(network_flow(g) != flow) {
		    printf("Error in mf_karzanov result.\n");
		    printf("%d != %d\n", network_flow(g), flow);
		    exit(1);
		}
		network_reset(g);
	    }
            timing_stop(t,3);
	    
	    /* Subtract the time spent reseting the network from the
	     * measurements.
	     */
	    timing_start();
	    for(j = 0; j < N_SAMPLES; j++) {
		network_reset(g);
	    }
	    timing_sub(t);

	    network_free(g);
	}

	/* Print line of results for the current value of n. */
	printf("%d", n);
	timing_print(t, "\t%.1f", N_GRAPHS * N_SAMPLES);
	putchar('\n');
    }

    timing_free(t);
    
    return 0;
}
#else
int main(void)
{
    network_t *g;
    timing_t *t;
    int i, j;
    int m;
    int flow;

    /* This computes the average time taken by the maximum flow algorithms over
     * several different random graphs of the same size.  This uses several
     * samples per random graph in order to improve the accuracy.  
     */
    t = timing_alloc(4);
    printf("Timing algorithms...");
    printf("\nCPU Time Results (msec):\n");
    printf("m\tFF\tDinic\tMPM\tKarzanov\n");
    for(m = STEP_EDGES; m <= MAX_EDGES; m += STEP_EDGES) {
        timing_reset(t);
	for(i = 0; i < N_GRAPHS; i++) {
	    g = network_rand(N_VERTICES, m, MIN_CAPACITY, MAX_CAPACITY);
/* 	    network_print(g); */
/* 	    exit(1); */
	    mf_ford(g);
	    flow = network_flow(g);
	    network_reset(g);

	    /* Time Ford Fulkerson */
            timing_start();
	    for(j = 0; j < N_SAMPLES; j++) {
/*		mf_ford(g);*/
/* 		network_print(g); */
/* 		if(network_flow(g) != flow) { */
/* 		    printf("Error in mf_ford result.\n"); */
/* 		    exit(1); */
/* 		} */
		network_reset(g);
	    }
            timing_stop(t,0);

	    /* Time Dinic */
            timing_start();
	    for(j = 0; j < N_SAMPLES; j++) {
		mf_dinic(g);
/* 		network_print(g); */
		if(network_flow(g) != flow) {
		    printf("Error in mf_dinic result.\n");
		    printf("%d != %d\n", network_flow(g), flow);
		    exit(1);
		}
		network_reset(g);
	    }
            timing_stop(t,1);

	    /* Time MPM */
            timing_start();
	    for(j = 0; j < N_SAMPLES; j++) {
/* 	        network_print(g); */
		mf_mpm(g);
		if(network_flow(g) != flow) {
		    printf("Error in mf_mpm result.\n");
		    exit(1);
		}
		network_reset(g);
	    }
            timing_stop(t,2);

	    /* Time Karzanov */
            timing_start();
	    for(j = 0; j < N_SAMPLES; j++) {
/* 	        network_print(g); */
		mf_karzanov(g);
		if(network_flow(g) != flow) {
		    printf("Error in mf_karzanov result.\n");
		    exit(1);
		}
		network_reset(g);
	    }
            timing_stop(t,3);

	    /* Subtract the time spend reseting the network from the
	     * measurements.
	     */
            timing_start();
	    for(j = 0; j < N_SAMPLES; j++) {
		network_reset(g);
	    }
            timing_sub(t);

	    network_free(g);
	}

	/* Print line of results for the current value of m. */
	printf("%d", m);
	timing_print(t, "\t%.2f", N_GRAPHS * N_SAMPLES);
	putchar('\n');
    }

    timing_free(t);
    
    return 0;
}
#endif

/*** --- End of Main Program --------------------------------------------- ***/
