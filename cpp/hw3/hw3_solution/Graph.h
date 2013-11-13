// Copyright 2013 Steve Gribble -- gribble (at) gmail (dot) com

#ifndef _HW3_GRAPH_H_
#define _HW3_GRAPH_H_

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <list>
#include <random>
#include <string>
#include <vector>
#include <inttypes.h>

#include "./UnionFindForest.h"

namespace hw3 {

// Represents the name of a graph node.
typedef uint32_t Node;

// A "Graph" is an undirected graph structure, containing a set of
// nodes and a set of edges between the nodes. As well, nodes and
// edges both have (templated) values associated with them.
// Internally, our implementation uses an edge list representation,
// which means it is more efficient for sparse graphs than dense
// graphs.
//
// NodeValue and EdgeValue represent values associated with nodes
// and edges in the graph. Much like STL containers, the Graph might
// internally make copies of and destroy copies of NodeValues and
// EdgeValues.
template <typename NodeValue, typename EdgeValue>
class Graph {
 public:
  // Constructs a random graph with the given number of nodes and
  // edge density, with edges values ranging randomly and uniformly
  // between [edge_min, edge_max). Each new node will be initialized
  // to have NodeValue val.
  static Graph<NodeValue, double>
    BuildRandomGraph(uint32_t num_nodes, double density, double edge_min,
                     double edge_max, NodeValue val);

  // Builds a graph out of a file. The file format must be an initial
  // integer that is the node size of the graph and the further values will
  // be integer triples: (i, j, cost). For example:
  //
  //   4
  //   0 1 12
  //   0 2 13
  //   1 0 12
  //   2 0 13
  //   2 3 5
  //   3 2 5
  //
  // If the file doesn't exist, isn't readable, or contains
  // ill-formatted data, this constructor will exit(EXIT_FAILURE).
  Graph(std::string filename);

  Graph() { }
  virtual ~Graph() { }

  uint32_t num_vertices(void) const { return num_vertices_; }
  uint32_t num_edges(void) const { return num_edges_; }

  // Returns true if Node "x" is adjacent to Node "y", false
  // otherwise. If x or y refer to nonexistent Nodes (i.e., they are
  // >= num_edges_), exit()'s the process with EXIT_FAILURE.
  bool AreNodesAdjacent(Node x, Node y) const;

  // Returns a list of Nodes that are direct neighbors of Node x. If x
  // refers to a nonexistent Node, exit()'s the process with
  // EXIT_FAILURE.
  std::list<Node> Neighbors(Node x) const;

  // Adds a new Node to the graph with NodeValue val to the
  // graph. Returns the name of the new Node.
  Node AddNode(NodeValue val);

  // Adds an (undirected) edge between Nodes x and y, if that edge
  // doesn't already exist, with EdgeValue val. exit()'s the process
  // with EXIT_FAILURE if x or y don't exist in the graph. Returns true
  // if a new edge was added, false if the edge already existed.
  bool AddEdge(Node x, Node y, EdgeValue val);

  // Gets/sets the NodeValue associated with Node x. If x doesn't
  // exist in the graph, exit()'s the process with EXIT_FAILURE.
  NodeValue get_node_value(Node x) const;
  void set_node_value(Node x, NodeValue val);

  // Gets/sets the EdgeValue associated with the edge between Nodes
  // x and y. If x or y don't exist in the graph, or if the edge
  // between them doesn't exist, exit()'s the process with
  // EXIT_FAILURE.
  EdgeValue get_edge_value(Node x, Node y) const;
  void set_edge_value(Node x, Node y, EdgeValue val);

  typedef struct {
    uint32_t from;
    uint32_t to;
    EdgeValue weight;
  } KruskalEdge;

  typedef struct {
    EdgeValue cost;
    std::vector<KruskalEdge> edges;
  } KruskalResult;

  // Runs Kruskal's algorithm to compute the minimum spanning tree
  // of this graph.
  KruskalResult ComputeMST() const;

 private:
  // The internal representation of an edge from some Node in the
  // graph to a neighboring Node. Each edge has a destination node
  // an EdgeValue associated with it.
  typedef struct {
    EdgeValue value;
    Node neighbor;
  } EdgeRep;

  // The internal representation of a Node in the graph. Each node
  // has a NodeValue, and a list of neighbors (i.e., a list of edges).
  typedef struct {
    NodeValue value;
    std::list<EdgeRep> neighbors;
  } NodeRep;

  // For each Node "x" in the graph, nodes_[x] is the NodeRep
  // representing that node.
  std::vector<NodeRep> nodes_;

  // Tracks the number of edges and verticies in the Graph.
  uint32_t num_vertices_ = 0;
  uint32_t num_edges_ = 0;
};

template <typename NodeValue, typename EdgeValue>
  Graph<NodeValue, EdgeValue>::Graph(std::string filename) {
  // Open up the file.
  std::ifstream f(filename);
  if (!f.is_open()) {
    std::cerr << "Graph::Graph(" << filename << "): couldn't "
              << "open the file for reading." << std::endl;
    exit(EXIT_FAILURE);
  }

  // Read the number of nodes.
  uint32_t num_nodes;
  f >> num_nodes;
  if (f.fail()) {
    std::cerr << "Graph::Graph(" << filename << "): couldn't "
              << "parse the number of nodes." << std::endl;
    exit(EXIT_FAILURE);
  }

  // Create the nodes.
  for (uint32_t i = 0; i < num_nodes; ++i) {
    AddNode(0);
  }

  // Read in the edges.
  while (true) {
    uint32_t source, dest;
    EdgeValue weight;
    f >> source;
    if (f.eof()) break;
    f >> dest;
    f >> weight;
    if (f.fail()) {
      std::cerr << "Graph::Graph(" << filename << "): couldn't "
                << "parse an edge triplet." << std::endl;
      exit(EXIT_FAILURE);
    }
    AddEdge(source, dest, weight);
  }
}

template <typename NodeValue, typename EdgeValue>
Graph<NodeValue, double> 
  Graph<NodeValue, EdgeValue>::BuildRandomGraph(uint32_t num_nodes,
                                                double density,
                                                double edge_min,
                                                double edge_max,
                                                NodeValue val) {
  Graph<NodeValue, double> retgraph;

  // Add the initial set of nodes.
  for (uint32_t i = 0; i < num_nodes; ++i) {
    retgraph.AddNode(val);
  }

  // Initialize our source of randomness. Relies on C++11 and having a
  // recent version of gcc.
  std::random_device rd;
  std::default_random_engine re(rd());
  std::uniform_real_distribution<double> edge_rand(edge_min, edge_max);
  std::uniform_real_distribution<double> density_rand(0.0, 1.0);

  // Go through and add edges.
  for (uint32_t source = 0; source < retgraph.num_vertices() - 1; ++source) {
    for (uint32_t dest = source + 1; dest < retgraph.num_vertices(); ++dest) {
      if (density_rand(re) < density) {
        // Add this edge, with a random edge value.
        retgraph.AddEdge(source, dest, edge_rand(re));
      }
    }
  }

  return retgraph;
}

template <typename NodeValue, typename EdgeValue>
bool Graph<NodeValue, EdgeValue>::AreNodesAdjacent(Node x, Node y) const {
  // Argument validation.
  if ((x >= num_vertices_) || (y >= num_vertices_)) {
    std::cerr << "Graph::AreNodesAdjacent called with non-present node(s): "
              << "(" << x << "," << y << "), num_vertices_ = " << num_vertices_
              << std::endl;
    exit(EXIT_FAILURE);
  }

  for (const auto &n : nodes_[x].neighbors) {
    if (n.neighbor == y)
      return true;
  }
  return false;
}

template <typename NodeValue, typename EdgeValue>
std::list<Node> Graph<NodeValue, EdgeValue>::Neighbors(Node x) const {
  // Argument validation.
  if (x >= num_vertices_) {
    std::cerr << "Graph::Neighbors called with non-present node: "
              << "(" << x << "), num_vertices_ = " << num_vertices_
              << std::endl;
    exit(EXIT_FAILURE);
  }

  std::list<Node> retset;
  for (const auto &n : nodes_[x].neighbors) {
    retset.push_back(n.neighbor);
  }

  return retset;
}

template <typename NodeValue, typename EdgeValue>
Node Graph<NodeValue, EdgeValue>::AddNode(NodeValue val) {
  NodeRep nr = {val};
  nodes_.push_back(nr);
  return num_vertices_++;
}

template <typename NodeValue, typename EdgeValue>
bool Graph<NodeValue, EdgeValue>::AddEdge(Node x, Node y, EdgeValue val) {
  // Argument validation.
  if ((x >= num_vertices_) || (y >= num_vertices_)) {
    std::cerr << "Graph::Neighbors called with non-present node(s): "
              << "(" << x << "," << y << "), num_vertices_ = " << num_vertices_
              << std::endl;
    exit(EXIT_FAILURE);
  }

  // See if the edge already exists.
  for (const auto &n : nodes_[x].neighbors) {
    if (n.neighbor == y)
      return false;
  }

  // The edge doesn't exist; add it (in both directions).
  EdgeRep e_xy = {val, y}, e_yx = {val, x};
  nodes_[x].neighbors.push_back(e_xy);
  nodes_[y].neighbors.push_back(e_yx);
  num_edges_++;
  return true;
}

template <typename NodeValue, typename EdgeValue>
NodeValue Graph<NodeValue, EdgeValue>::get_node_value(Node x) const {
  // Argument validation.
  if (x >= num_vertices_) {
    std::cerr << "Graph::get_node_value called with non-present node: "
              << "(" << x << "), num_vertices_ = " << num_vertices_
              << std::endl;
    exit(EXIT_FAILURE);
  }

  return nodes_[x].value;
}

template <typename NodeValue, typename EdgeValue>
void Graph<NodeValue, EdgeValue>::set_node_value(Node x, NodeValue val) {
  // Argument validation.
  if (x >= num_vertices_) {
    std::cerr << "Graph::set_node_value called with non-present node: "
              << "(" << x << "), num_vertices_ = " << num_vertices_
              << std::endl;
    exit(EXIT_FAILURE);
  }

  nodes_[x].value = val;
}

template <typename NodeValue, typename EdgeValue>
EdgeValue Graph<NodeValue, EdgeValue>::get_edge_value(Node x, Node y) const {
  // Argument validation.
  if ((x >= num_vertices_) || (y >= num_vertices_)) {
    std::cerr << "Graph::get_edge_value called with non-present node(s): "
              << "(" << x << "," << y << "), num_vertices_ = " << num_vertices_
              << std::endl;
    exit(EXIT_FAILURE);
  }

  for (const auto &n : nodes_[x].neighbors) {
    if (n.neighbor == y) {
      return n.value;
    }
  }

  // The edge doesn't exist.
  std::cerr << "Graph::get_edge_value called with non-present edge: "
            << "(" << x << "," << y << ")"
            << std::endl;
  exit(EXIT_FAILURE);

  // Never actually get here; this is just to make the compiler happy.
  return EdgeValue();
}

template <typename NodeValue, typename EdgeValue>
void Graph<NodeValue, EdgeValue>::set_edge_value(Node x, Node y, EdgeValue val) {
  // Argument validation.
  if ((x >= num_vertices_) || (y >= num_vertices_)) {
    std::cerr << "Graph::set_edge_value called with non-present node(s): "
              << "(" << x << "," << y << "), num_vertices_ = " << num_vertices_
              << std::endl;
    exit(EXIT_FAILURE);
  }

  // Set the edge value, in both directions.
  for (auto &n : nodes_[x].neighbors) {
    if (n.neighbor == y) {
      n.value = val;
      break;
    }
  }
  for (auto &n : nodes_[y].neighbors) {
    if (n.neighbor == x) {
      n.value = val;
      return;
    }
  }

  // The edge doesn't exist.
  std::cerr << "Graph::set_edge_value called with non-present edge: "
            << "(" << x << "," << y << ")"
            << std::endl;
  exit(EXIT_FAILURE);
}

template <typename NodeValue, typename EdgeValue>
typename Graph<NodeValue, EdgeValue>::KruskalResult
Graph<NodeValue, EdgeValue>::ComputeMST() const {
  KruskalResult res;
  res.cost = 0.0;

  // Create an ordered list of edges.
  std::vector<KruskalEdge> edges;
  for (uint32_t node = 0; node < nodes_.size(); ++node) {
    for (const auto &edge : nodes_[node].neighbors) {
      if (edge.neighbor > node) {
        KruskalEdge triplet = {node, edge.neighbor, edge.value};
        edges.push_back(triplet);
      }
    }
  }
  std::sort(edges.begin(), edges.end(),
            [](const KruskalEdge& a, const KruskalEdge& b) {
              return (a.weight < b.weight);
            });

  // Run Kruskal's algorithm.
  UnionFindForest uff(num_vertices());
  for (const auto &triplet : edges) {
    if (uff.Find(triplet.from) != uff.Find(triplet.to)) {
      res.edges.push_back(triplet);
      uff.Union(triplet.from, triplet.to);
      res.cost += triplet.weight;
    }
  }

  // Sort the resulting MST edge list by (from, to).
  std::sort(res.edges.begin(), res.edges.end(),
            [](const KruskalEdge& a, const KruskalEdge& b) {
              if (a.from < b.from)
                return true;
              if (a.from > b.from)
                return false;
              return (a.to < b.to);
            });

  return res;
}

} // end namespace hw3

#endif // _HW3_GRAPH_H_