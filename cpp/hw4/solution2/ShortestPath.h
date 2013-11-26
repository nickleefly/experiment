// Copyright 2013 Steve Gribble -- gribble (at) gmail (dot) com

#ifndef _HW4_SHORTESTPATH_H_
#define _HW4_SHORTESTPATH_H_

#include <cstdlib>
#include <iostream>
#include <map>
#include <set>
#include <utility>
#include <vector>
#include <inttypes.h>
#include "./Graph.h"

namespace hw4 {

// A "ShortestPath" is able to calculate the shortest paths between
// nodes in a Graph<NodeValue, double>.
//
// The correct sequence when using a ShortestPath is:
//
// (1) Allocate a ShortestPath object, passing in a Graph to its
//     constructor. (The caller retains ownership of the Graph;
//     ShortestPath takes a copy of the Graph.)
//
// (2) Use get_shortest_path(from, to, *path) to get the shortest path
//     from node "from" to node "to." Runs Dijkstra's algorithm to
//     calculate shortest paths from "from" to all other nodes, and
//     caches the result for quick lookup in the future.
//
// e.g.,
//
//     hw4::Graph<int,double> graph =
//       hw4::Graph<int,double>::BuildRandomGraph(10, 0.4, 1.0, 5.0, 0);
//
//     hw4::ShortestPath<int> sp(graph);
//
//     hw4::ShortestPath<int>::Path path;
//     if (sp.get_shortest_path(1, 2), &path) {
//       std::cout << "Hop count: " << path.Hopcount() << std::endl;
//       std::cout << "Path distance: " << path.Distance() << std::endl;
//     }
template <typename NodeValue>
class ShortestPath {
 public:

  // Represents a node in a path.
  class PathNode {
   public:
    PathNode(Node n, NodeValue v) : name(n), value(v) { }

    Node name;
    NodeValue value;
  };

  // Represents a path. Consider a Path p such as the following:
  //
  //     N0 <--E0--> N1 <--E1--> N2
  //
  // with the following values:
  //     N0: node name 0; node value 100
  //     E0: edge connecting (N0,N1); edge value 5.0
  //     N1: node name 1; node value 101
  //     E1: edge connecting (N1,n2); edge value 7.0
  //     N2: node name 2; node value 102
  //
  // Then, for example, we'd have:
  //     p.get_node(0) ==> a NodeValue with {0, 100}
  //     p.get_edge(0) ==> 5.0 -- connects node 0 and 1
  //     p.Hopcount() ==> 2
  //     p.Distance() ==> 12.0
  class Path {
   public:
    Path() { }
    virtual ~Path() { }

    // Adds an initial node to the path. If called when there are
    // already > 0 nodes in the path, exit()'s with EXIT_FAILURE.
    void AddInitialNode(PathNode node) {
      if (nodes_.size() != 0) {
        std::cerr << "ShortestPath::Path::AddInitialNode called, but "
                  << "path already has an initial node."
                  << std::endl;
        exit(EXIT_FAILURE);
      }
      nodes_.push_back(node);
    }

    // Adds a hop to the end of the path. If called when there isn't
    // at least an initial node in the path, exit()'s with
    // EXIT_FAILURE.
    void AddHopToEndOfPath(double edge, PathNode node) {
      if (nodes_.size() == 0) {
        std::cerr << "ShortestPath::Path::AddHopToEndOfPath called, "
                  << "but path doesn't have an initial node."
                  << std::endl;
        exit(EXIT_FAILURE);
      }
      edges_.push_back(edge);
      nodes_.push_back(node);
    }

    // Gets a node from the path. If called with a node offset that
    // doesn't exist in the path, exit()'s with EXIT_FAILURE.
    PathNode get_node(uint32_t offset) {
      if (offset >= nodes_.size()) {
        std::cerr << "ShortestPath::Path::get_node called with offset "
                  << "(" << offset << "), which is beyond the end of "
                  << "the path (Nodecount = " << Nodecount() << ")"
                  << std::endl;
        exit(EXIT_FAILURE);
      }
      return nodes_[offset];
    }

    // Gets an edge from the path. If called with an edge offset
    // that doesn't exist in the path, exit()'s with EXIT_FAILURE.
    double get_edge(uint32_t offset) {
      if (offset >= edges_.size()) {
        std::cerr << "ShortestPath::Path::get_edge called with offset "
                  << "(" << offset << "), which is beyond the end of "
                  << "the path (Hopcount = " << Hopcount() << ")"
                  << std::endl;
        exit(EXIT_FAILURE);
      }
      return edges_[offset];
    }

    // Returns the number of nodes in the path.
    uint32_t Nodecount(void) const { return nodes_.size(); }

    // Returns the number of hops (i.e., edges) in the path.
    uint32_t Hopcount(void) const { return edges_.size(); }

    // Returns the total distance of the path.
    double Distance(void) const {
      double sum = 0.0;
      for (double d : edges_) {
        sum += d;
      }
      return sum;
    }

   private:
    std::vector<PathNode> nodes_;
    std::vector<double> edges_;
  }; // end of class Path

  explicit ShortestPath(const Graph<NodeValue, double> &graph)
    : graph_(graph) { }

  virtual ~ShortestPath() { }

  // Returns (through the retpath return parameter) the shortest path
  // from node 'from' to node 'to'. If 'from' or 'to' are not in the
  // graph, exit()'s with EXIT_FAILURE.
  //
  // Returns true if a path from 'from' to 'to' exists, false otherwise.
  // If false is returned, then 'retpath' is untouched.
  bool get_shortest_path(Node from, Node to, Path *retpath);

  // Print out a representation of the shortest paths.
  void PrintPaths(void);

 private:
  // Calculates and caches all shortest paths leading from 'from'.
  void CalculateShortestPaths(Node from);

  // The graph over which we calculate shortest paths.
  Graph<NodeValue, double> graph_;

  // The cached calculated shortest paths; a map from std::pair<from,to>
  // to a Path.
  std::map<std::pair<Node, Node>, Path> shortest_paths_;
};

template <typename NodeValue>
void ShortestPath<NodeValue>::CalculateShortestPaths(Node from) {
  // If we've already calculated the shortest paths from this node,
  // short circuit the return.
  std::pair<Node,Node> testkey(from, from);
  if (shortest_paths_.find(testkey) != shortest_paths_.end())
    return;

  // A class representing the intermediate state of nodes in the
  // Dijkstra calculation.
  class DijkstraNodeState {
   public:
    Node name_;
    double distance_to_source_;
    Node previous_;

    DijkstraNodeState()
      : name_(0), distance_to_source_(0.0), previous_(0) { }

    DijkstraNodeState(Node name, double dist, Node previous)
      : name_(name), distance_to_source_(dist), previous_(previous) { }

    DijkstraNodeState(const DijkstraNodeState &copy_from_me) {
      name_ = copy_from_me.name_;
      distance_to_source_ = copy_from_me.distance_to_source_;
      previous_ = copy_from_me.previous_;
    }

    // A comparison function for the unvisited set. Note that we use
    // node names as tiebreakers; this is important, since two
    // different nodes can have the same distance to the source, and
    // we need a deterministic way to tiebreak them.
    static bool comp(DijkstraNodeState *lhs, DijkstraNodeState *rhs) {
      if (lhs->distance_to_source_ < rhs->distance_to_source_)
        return true;
      if (lhs->distance_to_source_ > rhs->distance_to_source_)
        return false;
      return lhs->name_ < rhs->name_;
    }
  };

  // The set of nodes over which we're running Dijkstra.
  std::unique_ptr<DijkstraNodeState[]>
    nodes(new DijkstraNodeState[graph_.num_vertices()]);

  // A std::set representing the priority queue of unvisited nodes.
  std::set<DijkstraNodeState*,
    bool(*)(DijkstraNodeState*,DijkstraNodeState*)>
    unvisited(DijkstraNodeState::comp);
  for (Node n = 0; n < graph_.num_vertices(); ++n) {
    // We use previous_ = graph_.num_vertices() as the special value
    // that previous_ has not yet been set.
    DijkstraNodeState dns(n,
                          n == from ?
                          0.0 : std::numeric_limits<double>::infinity(),
                          graph_.num_vertices());
    nodes[n] = dns;
    unvisited.insert(&nodes[n]);
  }

  // The heart of the Dijkstra algorithm.
  while (!unvisited.empty()) {
    // Find next node to consider.
    DijkstraNodeState *u = *unvisited.begin();
    unvisited.erase(unvisited.begin());
    if (u->distance_to_source_ == std::numeric_limits<double>::infinity())
      break;

    for (const auto v : graph_.Neighbors(u->name_)) {
      double altdist = u->distance_to_source_ +
        graph_.get_edge_value(u->name_, v);
      if (altdist < nodes[v].distance_to_source_) {
        // Pop v out of the set.
        auto v_it = unvisited.find(&nodes[v]);
        if (v_it == unvisited.end())
          continue;
        unvisited.erase(v_it);

        // Update v.
        nodes[v].distance_to_source_ = altdist;
        nodes[v].previous_ = u->name_;
        unvisited.insert(&nodes[v]);
      }
    }
  }

  // Populate the shortest_paths_ with the results.
  for (Node n = 0; n < graph_.num_vertices(); ++n) {
    // Special case the empty path.
    if (n == from) {
      Path p;
      p.AddInitialNode(PathNode(from, graph_.get_node_value(from)));
      shortest_paths_[std::make_pair(from, from)] = p;
      continue;
    }

    // Special case if there is no path back.
    if (nodes[n].previous_ == graph_.num_vertices()) {
      continue;
    }

    // Build the path back.
    Node nextnode = n;
    Path reverse;
    reverse.AddInitialNode(PathNode(n, graph_.get_node_value(n)));
    while (nextnode != from) {
      Node nexthop = nodes[nextnode].previous_;
      double edgedist = graph_.get_edge_value(nextnode, nexthop);
      reverse.AddHopToEndOfPath(edgedist,
                                PathNode(nexthop,
                                         graph_.get_node_value(nexthop)));
      nextnode = nexthop;
    }

    // Reverse the path (to get the forward path).
    Path p;
    p.AddInitialNode(reverse.get_node(reverse.Nodecount() - 1));
    // Add the hops
    for (uint32_t i = 0; i < reverse.Nodecount() - 1; i++) {
      uint32_t nextnode = reverse.Nodecount() - 2 - i;
      uint32_t nextedge = reverse.Hopcount() - 1 - i;
      p.AddHopToEndOfPath(reverse.get_edge(nextedge),
                          reverse.get_node(nextnode));
    }

    // Add the forward path to the set.
    shortest_paths_[std::make_pair(from, n)] = p;
  }
}

template <typename NodeValue>
bool ShortestPath<NodeValue>::get_shortest_path(Node from, Node to,
  ShortestPath<NodeValue>::Path *retpath) {
  // Check arguments.
  if ((from >= graph_.num_vertices()) || (to >= graph_.num_vertices())) {
    std::cerr << "ShortestPath::get_shortest_path() invoked with invalid "
              << "from or to. (from,to) = (" << from << "," << to << "); "
              << "graph_.num_vertices() = " << graph_.num_vertices()
              << std::endl;
    exit(EXIT_FAILURE);
  }

  // Run Dijkstra from this node, if we haven't already.
  CalculateShortestPaths(from);

  // See if we have a shortest path from-->to.
  auto res = shortest_paths_.find(std::make_pair(from, to));
  if (res == shortest_paths_.end())
    return false;

  // Yes, so return it.
  *retpath = res->second;
  return true;
}

template <typename NodeValue>
void ShortestPath<NodeValue>::PrintPaths(void) {
  // For each source vertex...
  for (hw4::Node i = 0; i < graph_.num_vertices(); ++i) {
    std::cout << "from " << i << ":" << std::endl;
    // For each destination vertex...
    for (hw4::Node j = 0; j < graph_.num_vertices(); ++j) {
      hw4::ShortestPath<int>::Path nextpath;
      std::cout << "  to " << j << ":" << std::endl;
      // Get and print the shortest path from "source" to "destination."
      if (get_shortest_path(i, j, &nextpath)) {
        std::cout << "    hops=" << nextpath.Hopcount() << ", "
                  << "dist=" << nextpath.Distance() << ": "
                  << nextpath.get_node(0).name;
        for (uint32_t hop = 0; hop < nextpath.Hopcount(); hop++) {
          std::cout << "<-" << nextpath.get_edge(hop) << "->"
                    << nextpath.get_node(hop+1).name;
        }
        std::cout << std::endl;
      } else {
        std::cout << "    <no path>" << std::endl;
      }
    }
  }  
}

} // end namespace hw4

#endif // _HW4_SHORTESTPATH_H_