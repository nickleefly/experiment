/*** File:  sc_test.c - Example program using Tarjan's Strongly Connected
 ***                    Components Algorithm
 ***/
/*
 *   Shane Saunders
 */
#include <stdio.h>
#include "sc.h"
#include "../graphs/dgraph.h"


int main(void)
{
    dgraph_t *g;
    sc_result_t *r;

    
    /* Create a random graph for the test. */
    g = dgraph_rnd_sparse(20, 2.2);
        /* parameters = (no of vertices, edge factor) */
    
    printf("Testing depth first search:\n\n");
    
    /* Note that the graph returned has all vertices reachable from vertex 0.
     * Try depth first search starting at vertex 0.
     */
    r = sc(g, 0);

    /* Display the result. */
    sc_result_print(r);

    /* Free space used for storing the result. */
    sc_result_free(r);

    
    /* Try starting from vertex 8. */
    putchar('\n');
    r = sc(g, 10);
    sc_result_print(r);
    sc_result_free(r);


    /* Free the graph. */
    dgraph_free(g);

    
    return 0;
}
