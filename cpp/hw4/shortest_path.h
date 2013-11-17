#ifndef SHORTEST_PATH_H

#define SHORTEST_PATH_H

#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <limits>

const bool kReverse = true; // defines the order of the priority queue. true = fast

// Manages the comparison in the the priority queue.
// The pair pair<int, double> is composed of an index and a distance.
// By default we set 'reverse' parameter to true in order to have the faster (lowest)
// accesible vertex on top.
class DistanceComparer{
public:
  DistanceComparer(const bool & reverse_param = true): reverse(reverse_param){}
  bool operator() (const pair<int,double> left_hand_side, const pair<int,double> right_hand_side){
    if(reverse)
      return (left_hand_side.second > right_hand_side.second);
    return (left_hand_side.second < right_hand_side.second);
  }
private:
  bool reverse;
};

// Implements the Dijkstra's algorithm by calling the 'path' method.
// If you want the cost of the shortest path between to points use 'path_size' instead
//
// sample usage:
//
// ShortestPath sp;
// Graph *g = new Graph(kNumber_nodes, kDensity_20_percent, 10);
// stack<int> shortestpath = path(g, source, target);
//
// or
//
// ShortestPath sp;
// Graph *g = new Graph(kNumber_nodes, kDensity_20_percent, 10);
// double cost = sp.path_size(g, 0, 49);
class ShortestPath{
public:
  vector<int> vertices(Graph* g); // list of vertices in Graph G.
  stack<int> path(Graph* g, const int source, const int target); //path(G, source, target): find shortest path in graph g between u-w and returns the sequence of vertices representing shorest path u-v1-v2-…-vn-w.
  double path_size(Graph* g, const int source, const int target); // path_size(source, targety): return the path cost associated with the shortest path.
  void prim(Graph* g); // Prim's algorithm that finds a minimum spanning tree for a connected weighted undirected graph

};

#endif // SHORTEST_PATH_H