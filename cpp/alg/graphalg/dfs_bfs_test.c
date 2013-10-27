/*** File:  dfs_bfs_test.c - Tests the DFS and BFS routines ***/
/*
 *   Shane Saunders
 */
#include <stdio.h>
#include "dfs_bfs.h"
#include "../graphs/dgraph.h"


int main(void)
{
    dgraph_t *g;
    dfs_bfs_result_t *r;

    
    /* Create a random graph. */
    g = dgraph_rnd_sparse(20, 2);

    
    printf("Testing depth first search:\n\n");
    
    /* Note that the graph returned has all vertices reachable from vertex 0.
     * Try depth first search starting at vertex 0.
     */
    r = dfs(g, 0);

    /* Display the result. */
    dfs_bfs_result_print(r);

    /* Free space used for storing the result. */
    dfs_bfs_result_free(r);

    
    /* Try starting from vertex 14. */
    putchar('\n');
    r = dfs(g, 14);
    dfs_bfs_result_print(r);
    dfs_bfs_result_free(r);


    /* Now test breadth first search. */
    printf("\nTesting breadth first search:\n\n");
    r = bfs(g, 0);
    dfs_bfs_result_print(r);
    dfs_bfs_result_free(r);
    putchar('\n');
    r = bfs(g, 14);
    dfs_bfs_result_print(r);
    dfs_bfs_result_free(r);
    

    /* Free the graph. */
    dgraph_free(g);

    
    return 0;
}
