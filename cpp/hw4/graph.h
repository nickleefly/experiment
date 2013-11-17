#ifndef GRAPH_H

#define GRAPH_H

#include <iostream> // std::cout, std::endl
#include <vector>
#include <queue>
#include <stack>

using namespace std;

const double kUndefined = -1; // use distance of -1 <=> undefined
const double kDensity_20_percent = 0.2; // graph density of 20%
const double kDensity_40_percent = 0.4; // graph density of 40%
const int kNumber_nodes = 50;  // number of nodes in graph
const int kRange_distance = 10; // range of distance of an edge

// simple data structure for Edge:
class Edge{
public:
  Edge(): cost(kUndefined){ }
  Edge(const int v):cost(v){}
  double cost; // distance
};

// Graph data structure:
//    sample usage:
//    Graph *g = new Graph(kNumber_nodes, kDensity_20_percent);
//    int number_vertices = g->V();
//    int number_edges = g->E();
//    bool isAdjacent = g->adjacent(0,9);
//    vector<Edge*> neighbors = g->neighbors(0);
//
//    g->add(0,1); // add edge in graph
//    g->remove(0,1); // remove edge in graph
//    double value = g->get_node_value(0); // get node value
//    g->set_node_value(0,1); // set node value
//    g3->get_edge_value(i,j); // get edge value
//    Edge *edgeptr = g->get_edge(0,2);
//    g->set_edge_value(1,3,edgeptr); // set edge value from another edge
//
//    Notes:
//    Representation of the Graph is done by using a N x N matrix called 'edges'.
//    Where the element edges[i,j] = x is an edge if (x > 0) and if x = 0 there is not edge.
//    This allows rapid updates and access for edge read, insertion and deletion.
//    It may have excessive space for graphs with many vertices and very few edges.

class Graph{
public:
  Graph():
    nvertices(0), nedges(0), edges(0, vector<Edge*>(0)), node_values(0,0), density_edge(1), range_distance(10){}
  Graph(const int number_nodes);
  Graph(const int nnodes, const double density_edge, const int range_dist);
  Graph(const string file_name); // build a graph from file
  ~Graph();
  void initialize();
  int V(); // returns the number of vertices in the graph
  int E(); // returns the number of edges in the graph
  bool adjacent(const int x, const int y); // tests whether there is an edge from node x to node y.
  vector<Edge*> neighbors(const int x); // lists all nodes y such that there is an edge from x to y.

  void add(const int x, const int y); // adds to G the edge from x to y, if it is not there.
  void remove(const int x, const int y); // removes the edge from x to y, if it is there.
  int get_node_value(const int x); // returns the value associated with the node x.
  void set_node_value(const int x, const int a); // sets the value associated with the node x to a.
  double get_edge_value(const int x, const int y); // returns the value associated to the edge (x,y).
  void set_edge_value(const int x, const int y, Edge *v); // sets the value associated to the edge (x,y) to v.
  Edge* get_edge(const int x, const int y);
  void print(); // prints adjacency matrix

private:
  int nvertices; // number of vertices
  int nedges; // number of edges
  vector< vector<Edge*> > edges; // adjacency information
  vector< int> node_values; // vector of nodes values (not needed for now..)
  double density_edge; // density parameter used at graph creation for density of edges.
  int range_distance; // range of distance between two vertex

  void extract_edge_info(string line, int &i, int& j, int &cost); // read line with format (i, j, cost) and set values into i, j, cost passed as ref.
};
#endif // GRAPH_H