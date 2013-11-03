#ifndef DIJKSTRA_H
#define DIJKSTRA_H
/* Dijkstra's Algorithm
 * ----------------------------------------------------------------------------
 */

class Heap;      // Heap
class HeapDesc;  // Heap descriptor
class DGraph;    // Graph

/* --- Dijkstra ---
 * Dijkstra's single-source algorithm.
 */
class Dijkstra {
  public:
    Dijkstra(int n, HeapDesc *heapD);
    ~Dijkstra();

    void init(const DGraph *g);
    void run(long *d, int s = 0);

  private:
    Heap *heap;        // pointer: heap
    bool *s;           // array: solution set state of vertices
    bool *f;           // array: frontier set state of vertices

    const DGraph *graph;    // pointer: directed graph
};


/* --- DijkstraDesc --
 * Dijkstra's algorithm descriptor class used for creating instances of
 * Dijkstra's algorithm.  The constructor parameter heapD points to a heap
 * descriptor which specifies the kind of heap to be used in the created
 * instances of Dijkstra's algorithm.
 */
class DijkstraDesc {
  public:
    DijkstraDesc(HeapDesc *heapD) : heapDesc(heapD) { };
    Dijkstra *newInstance(int n) const { return new Dijkstra(n, heapDesc); }

  private:
    HeapDesc *heapDesc;
};

#endif