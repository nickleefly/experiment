#include "dijkstra.h"
#include "dgraph.h"
#include "heaps/heap.h"
/* Dijkstra's Algorithm
 * ----------------------------------------------------------------------------
 */

/*--- Dijkstra -----------------------------------------------------------*/

/* - Constructor -
 * Allocate the algorithm for use on graphs of n vertices.  The parameter heapD
 * (points to a heap descriptor object) specifies then heap to be used by
 * Dijkstra's algorithm.
 */
Dijkstra::Dijkstra(int n, HeapDesc *heapD)
{
    heap = heapD->newInstance(n);
    s = new bool[n];
    f = new bool[n];
}

/* - Destructor - */
Dijkstra::~Dijkstra() {
    delete [] s;
    delete [] f;
    delete heap;
}

/* - init() -
 * Initialise the algorithm for use with the graph pointed to by g.
 */
void Dijkstra::init(const DGraph *g) {
    graph = g;
}

/* - run() -
 * Run the algorithm, computing single-source from the starting vertex v0.
 * This assumes that the array d has been initialised with d[v] = INFINITE_DIST
 * for all vertices v != v0.
 */
void Dijkstra::run(long *d, int v0)
{
    /* indexes, counters, pointers */
    int v, w;
    long dist;
    const DGraphEdge *edge;


  /*** initialisation ***/

    /* optimise access to the data structures allocated for the algorithm */
    const int n = graph->nVertices;
    const DGraphVertex *vertices = graph->vertices;

  /*** algorithm ***/

    /* initialise all vertices as unexplored */
    for(v = 0; v < n; v++) s[v] = false;
    for(v = 0; v < n; v++) f[v] = false;

    /* place v0 into the frontier set with a distance of zero */
    d[v0] = 0;
    heap->insert(v0, 0);
    f[v0] = true;

    /* repeatedly update distances from the minimum remaining trigger vertex */
    while(heap->nItems() > 0) {
        /* delete the vertex in frontier that has minimum distance */
        v = heap->deleteMin();

        /* the selected vertex moves from the frontier to the solution set */
        s[v] = true;
        f[v] = false;

        /* explore the OUT set of v */
        edge = vertices[v].outHead;
        while(edge) {
            w = edge->target;

            if(s[w] == false) {
                dist = d[v] + edge->dist;
                if(dist < d[w]) {
                    d[w] = dist;
                    if(f[w]) {
                        heap->decreaseKey(w, dist);
                    }
                    else {
                        heap->insert(w, dist);
                        f[w] = true;
                    }
                }
            }

            edge = edge->nextOut;
        } /* while */

    } /* while */
}