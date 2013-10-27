/***  File: da.c  ***/
/*
 *    Shane Saunders
 */
/* Implementations of Dijkstra's Algorithm. */

#include <stdlib.h>
#include "da.h"
#include "../timing/timing.h"
#include "../graphs/dgraph.h"

/*** Special values. ***/
#define TRUE 1
#define FALSE 0


/* heap_dijkstra() - Heap implementation of Dijkstra's algorithm.
 * Requires a pointer, g, to the directed graph used, a pointer to the starting
 * vertex, and a pointer to a da_heap_info_t structure for the heap used.
 * Returns a da_result_t structure containing the resulting shortest path
 * distances, and timing information.
 */
da_result_t *heap_dijkstra(const dgraph_t *g, int v0,
			   const heap_info_t *heap_info)
{
    int v, w;
    int *f, *s;
    long dist, *d;
    int n;
    
    dgraph_vertex_t *vertices;
    dgraph_edge_t *edge_ptr;
    
    void *front;
    int (*heap_delete_min)(void *);
    void (*heap_insert)(void *, int, long);
    void (*heap_decrease_key)(void *, int, long);
    int (*heap_n)(void *);
    void *(*heap_alloc)(int);
    void (*heap_free)(void *);
    
    da_result_t *result;
    long dist_comps;
    
    size_t int_size, long_size;


    /* Initialisation not specifically part of Dijkstra's algorithm. */
    int_size = sizeof(int);
    long_size = sizeof(long);
    heap_delete_min = heap_info->delete_min;
    heap_insert = heap_info->insert;
    heap_decrease_key = heap_info->decrease_key;
    heap_n = heap_info->n;
    heap_alloc = heap_info->alloc;
    heap_free = heap_info->free;

    /* Start of Dijkstra's algorithm. */
    dist_comps = 0;
    timer_start();
    vertices = g->vertices;
    result = malloc(sizeof(da_result_t));
    n = result->n = g->n;
    d = result->d = calloc(n, long_size);
    f = calloc(n, int_size);
    s = calloc(n, int_size);
    front = heap_alloc(n);

    /* The start vertex is part of the solution set. */
    s[v0] = TRUE;
    d[v0] = 0;

    /* Put out set of the starting vertex into the frontier and update the
     * distances to vertices in the out set.  k is the index for the out set.
     */
    edge_ptr = vertices[v0].first_edge;
    while(edge_ptr) {
        w = edge_ptr->vertex_no;
        dist = d[w] = edge_ptr->dist;
        heap_insert(front, w, dist);
#if DA_HEAP_DUMP
    heap_info->dump(front);
#endif
        f[w] = TRUE;
	edge_ptr = edge_ptr->next;
    }

    /* At this point we are assuming that all vertices are reachable from the
     * starting vertex and N > 1 so that j > 0.
     */

    while(heap_n(front) > 0) {

        /* Find the vertex in frontier that has minimum distance. */
        v = heap_delete_min(front);
#if DA_HEAP_DUMP
    heap_info->dump(front);
#endif
        
        /* Move this vertex from the frontier to the solution set.
         */
        s[v] = TRUE;
        f[v] = FALSE;

        /* Update distances to vertices, w, in the out set of v.
         */
        edge_ptr = vertices[v].first_edge;
	while(edge_ptr) {
	    w = edge_ptr->vertex_no;

            /* Only update if w is not already in the solution set.
             */
            if(!s[w]) {

                /* If w is in the frontier the new distance to w is the minimum
                 * of its current distance and the distance to w via v.
                 */
                dist = d[v] + edge_ptr->dist;
                if(f[w]) {
		    dist_comps++;
                    if(dist < d[w]) {
                        d[w] = dist;
                        heap_decrease_key(front, w, dist);
#if DA_HEAP_DUMP
    heap_info->dump(front);
#endif
 
                    }
                }
                else {
                    d[w] = dist;
                    heap_insert(front, w, dist);
#if DA_HEAP_DUMP
    heap_info->dump(front);
#endif
 
                    f[w] = TRUE;
                }
            } /* if */

	    edge_ptr = edge_ptr->next;
        } /* while */
    } /* while */

    /* End of Dijkstra's algorithm. */

    /* Record timing information. */
    result->ticks = timer_stop();
    result->key_comps = dist_comps + heap_info->key_comps(front);

    /* Free space used by arrays local to this function. */
    free(f);
    free(s);
    heap_free(front);

    return result;
}



/* da_result_free() - frees up space used by a da_result_t structure. */
void da_result_free(da_result_t *r)
{
    free(r->d);
    free(r);
}
