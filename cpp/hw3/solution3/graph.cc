/* Minimum Spanning Tree - Homework 3 - C++ for C programmers - Coursera 2013
 *
 * Output
 * ------
 *
 * $ g++-4.8 -std=c++11 -Wall -o hw3 hw3.cc
 * $ time ./hw3
 * * Minimum spanning tree on test graph:
 * Minimal weight: 30
 * Tree:
 * 20
 * 2 0 2
 * 6 1 1
 * 6 5 1
 * 7 4 1
 * 8 4 1
 * 9 2 1
 * 9 8 3
 * 10 7 2
 * 12 3 1
 * 12 9 3
 * 12 11 1
 * 13 9 3
 * 14 11 1
 * 15 4 2
 * 16 5 2
 * 17 1 1
 * 17 12 1
 * 18 14 1
 * 19 15 2
 *
 * * Results on 10000 simulations:
 *
 * Density 20: average shortest path = 6.82546
 *             average minimum spanning tree weight = 102.765
 * Density 40: average shortest path = 4.61254
 *            average minimum spanning tree weight = 75.8222
 *
 *      real0m8.444s
 *      user0m8.431s
 *      sys0m0.006s
 *
 * Correctness and efficiency
 * --------------------------
 *
 * Results are correct according to those found by my pairs on the discussion
 * forum. Implementation is efficient enought (8s without optimization in my
 * desktop computer for 40000 graph simulations (including Dijsktra and MST).
 * There are many points in my code that could be easely optimized, but I found
 * it was more clear and easy to read as it was. Don't hesitate if you have
 * comments. My code seems to be robust to ill-formated input, non connex
 * graphs, but I did'nt checked nor tested for really extremely unlikely cases
 * and some of them might still fail without warning.
 *
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <random>
#include <map>
#include <cassert>


const double INF = std::numeric_limits<double>::infinity();


/******************************************************************************
 *                              Graph Class                                   *
 *****************************************************************************/

/* Graph:
 *    A directed weighted graph structure. Nodes are exactly all the integers
 *    between `0` and `Graph.size() - 1`. Edges have a ``double`` non-negative
 *    weight, a weight is `INF` if and only if this edge is not in the graph.
 *    No self-loops are allowed.
 */
class Graph {

    // We use an adjacency matrix, keeping directly track of the weight of an
    // edge. This weight is `INF` if and only if this edge is not present in
    // the graph. Because the graph in undirected, we only store the lower
    // diagonal.
    std::vector<std::vector<double> > weight_matrix;

public:

    /* Graph:
     *   Initialize a graph of size `size` with no edges.
     */
    Graph(int size);

    /* Graph:
     *   Initialize a graph of size `size` by randomly selecting edges with
     *   weights in `weight_range` such as the final density is approximately
     *   `density`, making use of Mersenne Twister 19937 generator.
     */
    Graph(int size, double density, std::pair<double, double> weight_range,
      std::mt19937 &mt);

    /* Graph:
     *    Load a graph from file `file` formated as follow:
     *    First line:an integer which is the size of the graph
     *    All other line: an edge formated as
     *       source_node dest_node weight
     *    If the file is ill formated, print error message
     *    to std::cerr and return an empty `Graph` of size 0.
     */
    Graph(std::ifstream &file);

    /* size:
     *   Retrun the number of nodes in the graph.
     */
    inline int size() const {
    return weight_matrix.size();
    }

    /* weight:
     *   Return the weight of edge between nodes `i` and `j`.
     */
    inline double weight(int i, int j) const;

    /* set_weight:
     *   Set the weight of edge between nodes `i` and `j` to `wgh`. Nothing
     *   happen if `i == j` (no self loops)
     */
    inline void set_weight(int i, int j, double wgh);

    /* has_edge:
     *   Return true if edge (`i`, `j`) is in the graph.
     */
    inline bool has_edge(int i, int j) const {
    return weight(i, j) != INF;
    }

    /* outgoing_edges:
     *   Return a vector containing all edges that start on `node`, where an
     *   edge is only returned by the pair (destination_node, edge_cost)
     */
    std::vector< std::pair<int, double> > outgoing_edges(int node) const;

private:

    inline void initialize(int size);

};


Graph::Graph(int size) {
    initialize(size);
}

Graph::Graph(int size, double density, std::pair<double, double> weight_range,
         std::mt19937 &mt) {

    initialize(size);

    assert(weight_range.first >= 0.0);
    assert(weight_range.first <= weight_range.second);

    std::uniform_real_distribution<double> rand_float(0, 1);
    std::uniform_real_distribution<double> rand_range(weight_range.first,
                              weight_range.second);

    // For each possible edge (i, j) in the lower diagonal (strict)
    for (int i = 1; i < size; i++) {
    for (int j = 0; j < i; j++) {
        // Generate an edge ?
        if (rand_float(mt) <= density) {
        // With random weight
        double c = rand_range(mt);
        weight_matrix.at(i).at(j) = c;
        }
    }
    }
}

Graph::Graph(std::ifstream &file) {
    std::string line;
    if (not std::getline(file, line)) {
    std::cerr << "error: empty graph file" << std::endl;
    return;
    }
    std::istringstream line_stream(line);
    // First line must contain only the number of nodes
    int size;
    if (not (line_stream >> size)) {
    std::cerr << "error: first line of graph is not a valid size"
          << std::endl;
    return;
    }
    else {
    // We may now initialize the graph
    initialize(size);
    // And fill it with all the edges
    while (std::getline(file, line)) {
        std::istringstream line_stream(line);
        int source, dest;
        double weight;
        if (not (line_stream >> source)) {
        std::cerr << "error: invalid edge: " << line << std::endl;
        weight_matrix.clear();
        return;
        }
        if (source < 0 or source >= size) {
        std::cerr << "error: invalid source node: " << line << std::endl;
        weight_matrix.clear();
        return;
        }
        if (not (line_stream >> dest)) {
        std::cerr << "error: invalid edge: " << line << std::endl;
        weight_matrix.clear();
        return;
        }
        if (dest < 0 or dest >= size) {
        std::cerr << "error: invalid dest node: " << line << std::endl;
        weight_matrix.clear();
        return;
        }
        if (not (line_stream >> weight)) {
        std::cerr << "error: invalid edge: " << line << std::endl;
        weight_matrix.clear();
        return;
        }
        if (weight < 0) {
        std::cerr << "error: invalid weight (negative): " << line << std::endl;
        weight_matrix.clear();
        return;
        }
        set_weight(source, dest, weight);
    }
    }

}

inline double Graph::weight(int i, int j) const {
    if (i == j)
    return INF;
    // Recall cost_matrix is lower diagonal
    if (j > i)
    return weight_matrix.at(j).at(i);
    else
    return weight_matrix.at(i).at(j);
}

inline void Graph::set_weight(int i, int j, double wgh) {
    if (i == j)
    return;
    if (j > i)
    weight_matrix.at(j).at(i) = wgh;
    else
    weight_matrix.at(i).at(j) = wgh;
}


std::vector< std::pair<int, double> > Graph::outgoing_edges(int node) const {
    std::vector< std::pair<int, double> > res;
    for (int dest = 0; dest < size(); dest++) {
    if (has_edge(node, dest)) {
        res.push_back(std::pair<int, double>(dest,
                             weight(node, dest)));
    }
    }
    return res;
}

inline void Graph::initialize(int size) {
    // Initialize `weight_matrix` with `INF` everywhere (i.e. no edge).
    // Note we only store the lower triangular part.
    for (int i = 0; i < size; i++) {
    std::vector<double> zero_line(i, INF);
    weight_matrix.push_back(zero_line);
    }
}


/******************************************************************************
 *                        MinPriorityQueue Class                              *
 *****************************************************************************/

/* MinPriorityQueue:
 *   Implements a priority queue using a min-heap which allow efficient
 *   insertion, extraction and priority decrease. It is not a container,
 *   meaning it doesn't aim to store any elements, but to deal with their
 *   priority, hence the use of pointers for the element inserted
 *
 *   Note that this implementation is "incomplete" if you compare with
 *   standart ones. It has no creating method (on could use `insert` many
 *   time but this is usually not as efficient as having a particular method)
 *   nor an increase_priority method. However I those are straightforward to
 *   implement with this architecture and are omited for clarity of code
 *   because are not needed in this homework.
 */
template <typename T>
class MinPriorityQueue {

    // `heap` is a complete binary tree that always satisfy the min-heap
    // property, i.e. the priority of a node is greater than the one of it's
    // father.
    std::vector<std::pair<T*, double> > heap;

    // To keep track of where elements are in the tree and be able to find
    // them in O(1) when calling `decrease_priority`
    std::map<T*, unsigned> indexes;

 public:

    /* size:
     *   Return the number of elements in the heap.
     */
    inline int size() const {
    return heap.size();
    }

    /* has:
     *   Return true if element pointed by `element_p` is in the queue.
    */
    inline bool has(T* element_p) const {
    return indexes.find(element_p) != indexes.end();
    }

    /* insert:
     *   Insert element pointed by `element_p` in the queue with `priority`
     *   priority.
     */
    void insert(T* element_p, double priority);

    /* extract_min:
     *    Return a pointer to the element with minimal priority as well as it's
     *    priority and remove it from the queue.
     */
    std::pair<T*, double> extract_min();

    /* get_priority:
     *    Return the priority of element pointed by `element_p`, `INF` if not
     *    present.
     */
    inline double get_priority(T* element_p) const {
    if (has(element_p))
        return heap.at(indexes.find(element_p)->second).second;
    else
        return INF;
    }

    /* decrease_priority:
     *    Decrease priority of element pointed by `element_p` to
     *    `new_priority`, if lower, otherwise return.
     */
    void decrease_priority(T* element_p, double new_priority);

 private:
    // We keep things simpler. One could do faster by indexing heap from 1 and
    // use binary shift operators.
    // Next three short functions just hide the way the three structure of the
    // heap is implemented.
    inline unsigned father(unsigned idx) {
    return (idx - 1) / 2;
    }

    inline unsigned child1(unsigned idx) {
    return 2 * idx + 1;
    }

    inline unsigned child2(unsigned idx) {
    return 2 * idx + 2;
    }

    void swap(unsigned idx1, unsigned idx2);

    void percolate_up(unsigned idx);

    void percolate_down(unsigned idx);

};

template <typename T>
void MinPriorityQueue<T>::insert(T* element_p, double priority) {
    // First check element is not already in the queue
    if (has(element_p)) {
    std::cerr << "WARNING: element " << *element_p << " at address "
          << element_p << " is already in the queue. Ignored."
          << std::endl;
    }
    else {
    // Insert it a the end of the stack
    heap.push_back(std::pair<T*, double>(element_p, priority));
    // And in the index map
    indexes.insert(std::pair<T*, unsigned>(element_p, size() - 1));
    // And make sure min-heap property is satisfied
    percolate_up(size() - 1);
    }
}

template <typename T>
std::pair<T*, double> MinPriorityQueue<T>::extract_min() {
    // Check first heap is not empty
    if (size() == 0) {
    return std::pair<T*, double>(NULL, INF);
    }
    // Move it a the end and delete it
    swap(0, size() - 1);
    std::pair<T*, double> min_item = heap.back();
    heap.pop_back();
    indexes.erase(indexes.find(min_item.first));
    // Now the last element is at the root place, so we have to percolate
    // it down to satisfy the min-heap property
    percolate_down(0);
    return min_item;
}

template <typename T>
void MinPriorityQueue<T>::decrease_priority(T* element_p, double new_priority) {
    typename std::map<T*, unsigned>::const_iterator it;  // Or use ``auto``
    it = indexes.find(element_p);
    // First check element is indeed in the queue
    if (not has(element_p)) {
    std::cerr << "WARNING: no element " << *element_p << " at address "
          << element_p << " in the queue to decrease priority."
          << " Aborted." << std::endl;
    return;
    }
    unsigned idx = it->second;
    std::pair<T*, double> item = heap.at(idx);
    // Check we are decreasing priority, otherwise don't do anything.
    if (new_priority > item.second) {
    return;
    }
    heap.at(idx).second = new_priority;
    percolate_up(idx);
}

template <typename T>
void MinPriorityQueue<T>::swap(unsigned idx1, unsigned idx2) {
    std::pair<T*, double> tmp = heap.at(idx1); // ``auto`` nicer here also?
    heap.at(idx1) = heap.at(idx2);
    heap.at(idx2) = tmp;
    // Send this swapping information to the indexes map
    indexes.find(heap.at(idx1).first)->second = idx1;
    indexes.find(heap.at(idx2).first)->second = idx2;
}

template <typename T>
void MinPriorityQueue<T>::percolate_up(unsigned idx) {
    // While element has a smaller priority than it's father, push it up
    if (idx != 0 and heap.at(idx).second < heap.at(father(idx)).second) {
    swap(idx, father(idx));
    percolate_up(father(idx));
    }
}

template <typename T>
void MinPriorityQueue<T>::percolate_down(unsigned idx) {
    // While element has a larger priority than one of it's children, push
    // it down. Always swap with the child with smaller priority.
    unsigned min = idx;
    if (child1(idx) < size()
        and heap.at(min).second > heap.at(child1(idx)).second) {
        min = child1(idx);
    }
    if (child2(idx) < size()
        and heap.at(min).second > heap.at(child2(idx)).second) {
        min = child2(idx);
    }
    if (min != idx) {
        swap(min, idx);
        percolate_down(min);
    }
    }


std::ostream &operator<<(std::ostream &out, Graph &graph) {
    out << graph.size() << std::endl;
    for (int i = 0; i < graph.size(); i++) {
        for (int j = 0; j < i; j++) {
        if (graph.has_edge(i, j)) {
        out << i << " " << j << " " << graph.weight(i, j) << std::endl;
        }
    }
    }
    return out;
}

/******************************************************************************
 *                        Dijkstra Shortest Path                              *
 *****************************************************************************/

/* shortest_path:
 *   Return a vector containing the shortest path between `source` and every
 *   node in graph.
 *   Shortest path is computed using Dijsktra algorithm, since here all edges
 *   weights are positive (or here nonnegative)
 */
std::vector<double> shortest_path(const Graph &graph, int source) {

    // Shortest paths values for each node
    std::vector<double> res(graph.size(), INF);

    // Explicited nodes (to be able to point to them) in priority queue
    std::vector<int> nodes(graph.size());
    for (int i = 0; i < graph.size(); i++) {
    nodes.at(i) = i;
    }

    // Initialize the active nodes: all go in the queue
    MinPriorityQueue<int> queue;
    for (int i = 0; i < graph.size(); i++) {
    queue.insert(&nodes.at(i), INF);
    }

    // First, decrease priority of source to 0
    queue.decrease_priority(&nodes.at(source), 0);

    // Dijsktra algorithm
    while (queue.size() > 0) {
    std::pair<int*, double> min = queue.extract_min();
    // This node is not active anymore, it's priority is the shortest
    // distance we are looking for
    res.at(*min.first) = min.second;
    // Now decrease the priority of all it's neighbors. Note that if it
    // would in fact increase it: nothing would happen
    // (see `decrease_priority` implementation).
    // It is also possible that this node is not reachable from `source`
    // in this case it's priority is `INF` and there is no point to update
    // it's neighbors (in fact all the remaining nodes are not reachable
    // and their priority is `INF` so we could stop). This almost never
    // happen in homework 2, so I didn't add the test.
    std::vector< std::pair<int, double> > edges;
    edges = graph.outgoing_edges(*min.first);
    for (unsigned i = 0; i < edges.size(); i++) {
        int* node_p = &nodes.at(edges.at(i).first);
        double new_priority = min.second + edges.at(i).second;
        if (queue.has(node_p)) {
        queue.decrease_priority(node_p, new_priority);
        }
    }
    }

    return res;
}


/******************************************************************************
 *                Minimum Spanning Tree with - Prim algorithm                 *
 *****************************************************************************/

/* minimum_spanning_tree:
 *   Return the minimal spanning tree (a `Graph` instance that is a tree) for
 *   the graph `graph`, as as it's weight. If `graph` is not connex, returned
 *   tree will only cover the connex composant of node `0` and tree weight will
 *   be `INF`
 */
std::pair<Graph, double> minimum_spanning_tree(const Graph &graph) {

    Graph tree(graph.size());
    double min_weight = 0.0;

    static const int root = 0;  // But could be choosed randomly

    // Explicited nodes (to be able to point to them) in priority queue
    std::vector<int> nodes(graph.size());
    for (int i = 0; i < graph.size(); i++) {
    nodes.at(i) = i;
    }
    // Father of a node, to be able to reconstruct the tree after Prim's
    // algorithm. Only root will be allowed to have an `-1` father at
    // the end, otherwise the graph was not connex.
    std::vector<int> father(graph.size(), -1);

    // Priority queue. All nodes go in the queue, when a node is extracted from
    // the queue, it is (virtually) added to the tree. Each node in the queue
    // has a priority which is the minimal weight between itself and any node
    // of the tree (it's father). In other words, if we extract node `u`, and
    // if a this moment it's father is `v` (v is know to be now in the tree,
    // and not in the queue anymore), then edge (u, v) is the edge with minimal
    // weight that join the partial built tree and the nodes in the queue.
    MinPriorityQueue<int> queue;
    for (int i = 0; i < graph.size(); i++) {
    queue.insert(&nodes.at(i), INF);
    }

    // First, decrease priority of root to 0
    queue.decrease_priority(&nodes.at(root), 0);

    // Prim algorithm
    while (queue.size() > 0) {
    std::pair<int*, double> min = queue.extract_min();
    // First of all, if the node we extract has an `INF` weight, this mean
    // it is not reachable from `root`, so stop here.
    if (min.second == INF) {
        min_weight = INF;
        break;
    }
    // Let's call `u` the node extracted, from now on in the tree. For each
    // edge `(u, v)` where `v`  is still in the queue, update the priority
    // of `v` with the weight of the edge `(u, v)`. If we find a smaller
    // value, then make `u` the new father of `v`.
    std::vector< std::pair<int, double> > edges;
    edges = graph.outgoing_edges(*min.first);
    for (auto it = edges.begin(); it != edges.end(); it++) {
        int* node_p = &nodes.at(it->first);
        double edge_weight = it->second;
        if (queue.has(node_p)
        and queue.get_priority(node_p) > edge_weight) {
        queue.decrease_priority(node_p, edge_weight);
        father.at(*node_p) = *min.first;
        }
    }
    // Let's now "put" `u` in the tree, we know the minimal edge is
    // (u, father[u]) with weight priority(u)
    if (nodes.at(*min.first) == root) {
        continue;
    }
    else {
        tree.set_weight(nodes.at(*min.first),
                father.at(*min.first),
                min.second);
        min_weight += min.second;
    }
    }

    return std::pair<Graph, double>(tree, min_weight);
}


/******************************************************************************
 *                                 Utils                                      *
 *****************************************************************************/

/* average:
 *   Return the average of `vec` ignoring possible `INF` values.
 */
double average(const std::vector<double> &vec) {
    double sum = 0.0;
    int cnt = 0; // Effective number of non `INF` values
    for (unsigned i = 0; i < vec.size(); i++) {
    if (vec.at(i) < INF) {
        sum += vec.at(i);
        cnt++;
    }
    //else: This happen six time for density20 with `mt` seed at 42
    }
    if (cnt == 0)
    return 0;
    else
    return sum / cnt;
}


/******************************************************************************
 *                                 Main                                       *
 *****************************************************************************/

int main() {

    // "Better" generator for randomness than usual `rand()`
    std::mt19937 mt(42);

    // Default parameters from homework 2 assingment
    const int N = 50;
    const double density20 = 0.20;
    const double density40 = 0.40;
    const std::pair<double, double> weight_range(1.0, 10.0);

    const unsigned nb_simulations = 10000;

    // Result on test graph provided on the course website
    std::ifstream test_graph_file("test.graph");
    if (test_graph_file.is_open()) {
    Graph g(test_graph_file);
    if (g.size() > 0) {
        std::pair<Graph, double> mst = minimum_spanning_tree(g);
        std::cout << "* Minimum spanning tree on test graph:" << std::endl;
        std::cout << "Minimal weight: " << mst.second << std::endl
              << "Tree: " << std::endl
              << mst.first << std::endl;
    }
    test_graph_file.close();
    }
    else {
    std::cerr << "Error: unable to open file \"test.graph\"\n\n";
    }

    // Result on Monte-Carlo simulations
    std::cout << "* Results on " << nb_simulations << " simulations: \n\n";
    // density20
    double sum_path = 0.0;
    std::vector<double> mst_weights;
    for (unsigned sim = 0; sim < nb_simulations; sim++) {
    Graph g = Graph(N, density20, weight_range, mt);
    std::vector<double> shorts = shortest_path(g, 0);
    mst_weights.push_back(minimum_spanning_tree(g).second);
    sum_path += average(shorts);
    }
    std::cout << "Density 20: average shortest path = "
          << sum_path / nb_simulations << std::endl;
    std::cout << "            average minimum spanning tree weight = "
          << average(mst_weights) << std::endl;
    // density40
    sum_path = 0.0;
    mst_weights.clear();
    for (unsigned sim = 0; sim < nb_simulations; sim++) {
    Graph g = Graph(N, density40, weight_range, mt);
    std::vector<double> shorts = shortest_path(g, 0);
    mst_weights.push_back(minimum_spanning_tree(g).second);
    sum_path += average(shorts);
    }
    std::cout << "Density 40: average shortest path = "
          << sum_path / nb_simulations << std::endl;
    std::cout << "            average minimum spanning tree weight = "
          << average(mst_weights) << std::endl;

    return 0;
}