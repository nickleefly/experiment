// shortest_path.cpp:
// Description: Dijkstra’s algorithm and Prim algorithm

#include "graph.h"
#include "shortest_path.h"

#include <algorithm>    // std::find

// list of vertices in Graph G.
vector<int> ShortestPath::vertices(Graph* g){
  vector<int> vertices;
  for(int i = 0; i < g->V(); ++i)
    vertices.push_back(i);
  return vertices;
}

// path(G, source, target): find shortest path between source-target
// and returns the sequence of vertices representing shorest path
//  source-v1-v2-…-vn-target.
stack<int> ShortestPath::path(Graph* g, const int source, const int target){
  // sortest path algorithm here

  // initialization
  typedef priority_queue< pair<int,double>, vector< pair<int,double> >, DistanceComparer> PriorityQueue;

  PriorityQueue q(kReverse); // reverse (faster first)

  vector<double> distances;
  vector<bool> visited;
  vector<int> previous;

  // Initializations
  vector<int> graph_vertices = vertices(g);
  for(int i = 0; i < graph_vertices.size(); ++i){
    // set unknown distance (ie: infinity) from source to i
    distances.push_back(numeric_limits<double>::max());
    visited.push_back(false); // nodes have not been visited
    previous.push_back(kUndefined); // set previous node in optimal path to undifined
  }

  distances[source] = 0; // distance from source to source
  q.push(make_pair(source, distances[source])); // start with the source node

  int current_vertex;
  while( !q.empty()){
    // vertex in Q with smallest distance and that has not yet neen visited
    current_vertex = q.top().first;
    q.pop();
    if(visited[current_vertex]){
      continue;
    }
    if(current_vertex == target){
      // shortes path found!
      break;
    }
    visited[current_vertex] = true; // mark node as visited

    // for each neighbor v of current_vertex
    double accumulate_distance = 0;
    vector<Edge*> neighbor = g->neighbors(current_vertex);
    for(int i = 0; i < neighbor.size(); ++i){
      if(neighbor[i]){
    accumulate_distance = distances[current_vertex] + neighbor[i]->cost;
    if(accumulate_distance < distances[i] && !visited[i]){
      distances[i] = accumulate_distance;
      previous[i] = current_vertex;
      q.push(make_pair(i, distances[i]));
    }
      }
    }
  }
  // build path sequence
  stack<int> sequence;
  current_vertex = target;
  while(previous[current_vertex] != kUndefined){
    sequence.push(current_vertex);
    current_vertex = previous[current_vertex];
  }
  sequence.push(current_vertex); // push that last element

  return sequence;
}

// path_size(G, source, target): return the path cost associated with the shortest path.
double ShortestPath::path_size(Graph* g, const int source, const int target){

  // get the shortest path
  stack<int> shortestpath = path(g, source, target);

  // compute the cost by going through all vertices in the path.
  double cost = 0;
  int point_a = shortestpath.top();
  shortestpath.pop();
  int point_b;
  while (!shortestpath.empty())
  {
    point_b = shortestpath.top();
    shortestpath.pop();
    cost += g->get_edge_value(point_a, point_b);

    point_a = point_b; // one step
  }
  return cost;
}

// the Prim algorithm
void ShortestPath::prim(Graph*g){

  // array of distances from source to each vertex
  // set every distance to infinity until we discover way to link a vertex to a spanning tree
  vector<double>dist(g->V()-1, numeric_limits<double>::max());

  // array indicating for a given vertex, which vertex in the tree it is closest to.
  // set every edge to undefined until we discover way to link a vertex to a spanning tree
  vector<int> edges(g->V()-1, kUndefined);

  // vector of finished vertices.
  vector<int> mst;

  // vector of unfinished vertices.
  vector<int> U;
  for(int i = 0; i < g->V(); i++){
    U.push_back(i);
  }
  // cost
  double cost(0);
  // pick a vertex s:
  int s = 0;
  dist[s] = 0;
  mst.push_back(s);
  while(mst.size() != g->V()){ // mst is missing a vertex

    // pick the vertex v in U with the shortest edge to the group of vertices in the MST
    int v;
    double current_distance;
    double min_distance = numeric_limits<double>::max();

    double shortest_edge = numeric_limits<double>::max();
    for(int i = 0; i < U.size(); i++){
      for(int j = 0; j < U.size(); j++){
    if((find(mst.begin(), mst.end(), U[i]) == mst.end()) &&
       (find(mst.begin(), mst.end(), U[j]) != mst.end())){
      current_distance = g->get_edge_value(U[i], U[j]);
      if( (current_distance != 0) && current_distance < min_distance){
        min_distance = current_distance;
        v = U[i];
      }
    }
      }
    }

    // v is now the vertex with the shortest edge to the group of vertices in the MST.
    // we add now v to mst.
    cost += min_distance;
    mst.push_back(v);
    vector<Edge*> neighbors = g->neighbors(v);
    // loop through every neighbor in v
    // check to see if that neighbor could reach the MST faster through v
    // than by linking through previous vertex.
    for(int i = 0; i < neighbors.size(); i++){
      if(neighbors[i] != NULL){
    if(neighbors[i]->cost < dist[i]){
      dist[i] = neighbors[i]->cost;
      edges[i] = v;
    }
      }
    }
  }
  // output edges and cost
  cout << "MST: ";
  for(int i = 0 ; i < mst.size(); i++){
    cout << mst[i] << " " ;
  }
  cout << endl;
  cout << "cost: " << cost << endl;
}