#include <algorithm>
#include <set>
#include <vector>
#include <stdexcept>
#include <istream>
#include <ostream>
#include <iostream>
#include <fstream>
#include <limits>

#include <cstdlib>
#include <ctime>

using namespace std;

// we assume that we have undirected graph and any 2 nodes connected with maximum 1 edge
class UndirectedGraph {
public:
    typedef size_t Vertex;                                          // vertex identified by a number from 0 to N - 1
    typedef double Value;
    struct Edge {
        Edge (Vertex from, Vertex to, Value value = 0) : from (from), to (to), value (value) {
        }

        Vertex from;
        Vertex to;
        Value value;
    };

    explicit UndirectedGraph (size_t nodes_count);                  // creates unconnected graph with nodes_count nodes
    explicit UndirectedGraph (const std::string& file_name);        // reads graph from a file
    explicit UndirectedGraph (istream& data);                       // reads graph from a stream

    size_t value () const;                                          // returns number of vertices in the graph
    size_t edges () const;                                          // returns number of edges in the graph
    vector<Edge> edges_list () const;                               // returns list of all edges in graph
    bool adjacent (Vertex x, Vertex y) const;                       // tests whether there is an edge from node x to node y
    vector<Vertex> neighbors (Vertex x) const;                      // lists all nodes y such that there is an edge from x to y
    void add_edge (Vertex x, Vertex y, Value value = 0);            // adds to G the edge from x to y with default value = 0
    void delete_edge (Vertex x, Vertex y);                          // removes the edge from x to y
    Value get_node_value (Vertex x) const;                          // returns the value associated with the node x
    void set_node_value (Vertex x, Value v);                        // sets the value associated with the node x to a
    Value get_edge_value (Vertex x, Vertex y) const;                // returns the value associated to the edge (x,y)
    void set_edge_value (Vertex x, Vertex y, Value v);              // sets the value associated to the edge (x,y) to v.

    friend ostream& operator<< (ostream& out, UndirectedGraph graph);

private:
    void init (istream& data);                                      // reads graph from a stream

    typedef set<Edge> GraphRepresentation;
    typedef set<Edge>::const_iterator GraphIterator;
    GraphRepresentation m_graph;
    size_t m_nodes_count;
};

bool operator < (const UndirectedGraph::Edge& left, const UndirectedGraph::Edge& right) {
    if (left.from != right.from)
        return left.from < right.from;

    return left.to < right.to;
}

UndirectedGraph::UndirectedGraph (size_t nodes_count) : m_nodes_count (nodes_count) {
}

UndirectedGraph::UndirectedGraph (const std::string& file_name) {
    ifstream file_stream (file_name.c_str ());

    this->init (file_stream);
}

UndirectedGraph::UndirectedGraph (istream& data) {
    this->init (data);
}

void UndirectedGraph::init (istream& stream) {
    if (!stream)
        throw runtime_error ("bad input");

    size_t graph_size = 0;
    stream >> graph_size;
    if (!graph_size)
        throw runtime_error ("bad graph size");

    UndirectedGraph::Vertex from = 0;
    UndirectedGraph::Vertex to = 0;
    UndirectedGraph::Value value = 0;
    while (stream) {
        stream >> from >> to >> value;
        m_graph.insert (Edge (from, to, value));
    }

    if (m_graph.empty ())
        m_nodes_count = 0;
    else
        m_nodes_count = (--m_graph.end ())->from + 1; // graph should be connected, so last edge has node with max number
}

size_t UndirectedGraph::value () const {
    return m_nodes_count;
}

size_t UndirectedGraph::edges () const {
    size_t cycles = 0;
    for (size_t i = 0; i < m_nodes_count; ++i)
        if (this->adjacent (i, i))
            ++cycles;

    return cycles + (m_graph.size () - cycles) / 2;
}

// as soon as our graph undirected we return only edges where from <= to
vector<UndirectedGraph::Edge> UndirectedGraph::edges_list () const {
    vector<Edge> edges;
    edges.reserve (this->edges () / 2);
    for (GraphRepresentation::const_iterator it = m_graph.begin (); it != m_graph.end (); ++it)
        if (it->from <= it->to)
            edges.push_back (*it);

    return edges;
}

bool UndirectedGraph::adjacent (Vertex x, Vertex y) const {
    return m_graph.find (Edge (x, y)) != m_graph.end ();
}

vector<UndirectedGraph::Vertex> UndirectedGraph::neighbors (Vertex x) const {
    vector<Vertex> result;
    GraphIterator it = m_graph.lower_bound (Edge (x, 0));
    while (it != m_graph.end ())
        if (it->from == x) {
            result.push_back (it->to);
            ++it;
        }
        else
            return result;

    return result;
}

void UndirectedGraph::add_edge (Vertex x, Vertex y, Value value) {
    this->delete_edge (x, y);

    m_graph.insert (Edge (x, y, value));
    if (x != y)
        m_graph.insert (Edge (y, x, value));
}

void UndirectedGraph::delete_edge (Vertex x, Vertex y) {
    m_graph.erase (Edge (x, y, 0));
    if (x != y)
        m_graph.erase (Edge (y, x, 0));
}

UndirectedGraph::Value UndirectedGraph::get_node_value (Vertex x) const {
    return this->get_edge_value (x, x);
}

void UndirectedGraph::set_node_value (Vertex x, Value value) {
    this->add_edge (x, x, value);
}

UndirectedGraph::Value UndirectedGraph::get_edge_value (Vertex x, Vertex y) const {
    GraphIterator it = m_graph.find (Edge (x, y));
    return it != m_graph.end () ? it->value : 0;
}

void UndirectedGraph::set_edge_value (Vertex x, Vertex y, Value v) {
    this->add_edge (x, y, v);
}

// prints graph representation as set of pairs
ostream& operator<< (ostream& out, UndirectedGraph graph) {
    out << "--------\n";
    for (UndirectedGraph::GraphIterator it = graph.m_graph.begin (); it != graph.m_graph.end (); ++it)
        if (it->from < it->to)
            out << "[(" << it->from << "," << it->to << ") : " << it->value << "]\n";
    out << "--------\n";

    return out;
}

// disjoint set union
// see http://en.wikipedia.org/wiki/Disjoint-set_data_structure
class DSUStructure {
public:
    DSUStructure (size_t cardinality);                                      // creates 'cardinality' disjoint sets

    UndirectedGraph::Vertex find_set (UndirectedGraph::Vertex node);        // returns the representative of the set that node belongs to
    void union_sets (UndirectedGraph::Vertex x, UndirectedGraph::Vertex y); // joins two subsets into a single subset

private:
    std::vector<UndirectedGraph::Vertex> m_parents;
};

DSUStructure::DSUStructure (size_t cardinality) : m_parents (cardinality) {
    for (size_t i = 0; i < cardinality; ++i)
        m_parents[i] = i;
}

UndirectedGraph::Vertex DSUStructure::find_set (UndirectedGraph::Vertex node) {
    if (m_parents[node] == node)
        return node;

    return m_parents[node] = this->find_set (m_parents[node]);
}

void DSUStructure::union_sets (UndirectedGraph::Vertex x, UndirectedGraph::Vertex y) {
    x = this->find_set (x);
    y = this->find_set (y);
    if (x == y)
        return;

    if (::rand () % 2)
        m_parents[x] = y;
    else
        m_parents[y] = x;
}

// class represents minimum spanning tree for given graph using Kruskal's_algorithm
class KruskalMST {
public:
    KruskalMST (const UndirectedGraph& graph);          // build MST tree

    UndirectedGraph::Value cost () const;               // get MST tree cost
    vector<UndirectedGraph::Edge> tree () const;        // get MST tree

    friend ostream& operator << (ostream& out, const KruskalMST& mst);

private:
    bool good_edge (const UndirectedGraph::Edge& edge); // edge should be added to MST
    void add_edge (const UndirectedGraph::Edge& edge);  // add edge to MST
    bool tree_built (size_t graph_size) const;          // returns true if we have full tree

private:
    vector<UndirectedGraph::Edge> m_tree;
    typedef vector<UndirectedGraph::Edge>::const_iterator EdgeIterator;
    mutable UndirectedGraph::Value m_cost;

    DSUStructure m_sets;
};

struct MSTEdgeSorter {
    bool operator () (const UndirectedGraph::Edge& left, const UndirectedGraph::Edge& right) {
        return left.value < right.value;
    }
};

KruskalMST::KruskalMST (const UndirectedGraph& graph) : m_cost (0), m_sets (graph.value ()) {
    vector<UndirectedGraph::Edge> edges = graph.edges_list ();
    std::sort (edges.begin (), edges.end (), MSTEdgeSorter ());

    size_t graph_size = graph.value ();
    for (EdgeIterator it = edges.begin (); it != edges.end (); ++it)
        if (this->good_edge (*it)) {
            this->add_edge (*it);
            if (this->tree_built (graph_size))
                break;
        }

    std::sort (m_tree.begin (), m_tree.end ());
}

UndirectedGraph::Value KruskalMST::cost () const {
    if (m_cost == 0)
        for (EdgeIterator it = m_tree.begin (); it != m_tree.end (); ++it)
            m_cost += it->value;

    return m_cost;
}

vector<UndirectedGraph::Edge> KruskalMST::tree () const {
    return m_tree;
}

bool KruskalMST::good_edge (const UndirectedGraph::Edge& edge) {
    return m_sets.find_set (edge.from) != m_sets.find_set (edge.to);
}

void KruskalMST::add_edge (const UndirectedGraph::Edge& edge) {
    m_sets.union_sets (edge.from, edge.to);
    m_tree.push_back (edge);
}

bool KruskalMST::tree_built (size_t graph_size) const {
    return m_tree.size () == (graph_size - 1);
}

ostream& operator << (ostream& out, const KruskalMST& mst) {
    out << "MST cost is: " << mst.cost () << "\n";
    for (KruskalMST::EdgeIterator it = mst.m_tree.begin (); it != mst.m_tree.end (); ++it)
        out << "(" << it->from << "," << it->to << ") value is: " << it->value << "\n";

    return out;
}

int main () {
    UndirectedGraph graph ("hw3_testdata.txt");
    KruskalMST algo (graph);

    cout << algo;

    return 0;
}