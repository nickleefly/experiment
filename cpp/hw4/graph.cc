// Description: Graph ATD

#include "graph.h"

#include <fstream>

// Graph constructor:
// This is the main constructor, pass a density parameter to create a random graph.
Graph::Graph(const int number_vertices, const double dense_edge, const int range_dist):
  nedges(0), nvertices(number_vertices), edges(number_vertices, vector<Edge*>(number_vertices)), node_values(number_vertices, 0), density_edge(dense_edge), range_distance(range_dist){

  // Initialize random seed
  srand(time(NULL));

  // Initialize graph with random density and distance
  double random_distance;
  for(int i = 0; i < edges.size(); ++i){
    nvertices++;
    for(int j = i; j < edges.size(); ++j){
      if(i == j){
    edges[i][j] = NULL; // null pointer:  no loops in Graph
      } else {
    // manage density in graph
    if( ((double) rand() / (RAND_MAX)) < density_edge){
      // generate random number between 1 and range_distance
      random_distance = rand() %range_distance + 1;
      edges[i][j] = new Edge(random_distance);
      edges[j][i] = edges[i][j]; // undirected graph, connect both vertex
      nedges++;
    }
      }
    }
  }
}

// Graph constructor:
// This constructor an empty graph that you can then create by hand.
Graph::Graph(const int number_vertices):
  nedges(0), nvertices(number_vertices), edges(number_vertices, vector<Edge*>(number_vertices)), node_values(number_vertices, 0), density_edge(0), range_distance(0){}

// Graph constructor:
// Read in a graph from file.
// The file format will be an initial integer that is the node size of the graph
// and the further values will be integer triples: (i, j, cost).
Graph::Graph(const string file_name):nedges(0), density_edge(0), range_distance(0){

  ifstream file(file_name);

  if(file.is_open()){

    int read_number_vertices(0);
    int number_of_lines(0);
    int i, j, cost;

    string line, rest;
    int next_space = 0;

    while(getline(file, line)){
      if(number_of_lines == 0){
        read_number_vertices = atoi(line.c_str());
    // init Graph variables:
    nvertices = read_number_vertices;
    edges = vector< vector<Edge*> >(nvertices, vector<Edge*>(nvertices));
    node_values = vector<int>(nvertices, 0);
      } else {
    extract_edge_info(line, i, j , cost);

    // build current edge
    edges[i][j] = new Edge(cost);
    edges[j][i] = new Edge(cost); // undirected graph
      }
      number_of_lines++;
    }
    file.close();
  } else{
    cout << "unable to open file " << file_name << endl;
  }

}

// Graph destructor
Graph::~Graph(){
  for(int i = 0; i < edges.size(); i++){
    for(int j = i; j < edges.size(); j++){
      if(edges[i][j] != NULL){
    delete edges[i][j];
    edges[i][j] = edges[j][i]= NULL;
      }
    }
  }
}

// read line with format (i, j, cost) and set values into i, j, cost passed as ref.
void Graph::extract_edge_info(string line, int& i, int& j, int& cost){

    int next_space(0); // position of next space in string.
    string rest; // rest of string to parse.

    // get position of first space position
    // read i: extract info from line
    next_space= line.find(' ');
    i = atoi(line.substr(0, next_space).c_str());

    // keep track of rest of string
    rest = line.substr(next_space + 1,  string::npos);

    // update space position
    next_space = rest.find(' ');

    // read j: extract info from rest of line
    j = atoi(rest.substr(0, next_space).c_str());

    // keep track of rest of string
    rest = rest.substr(next_space + 1, string::npos);
    // update space positon
    next_space = rest.find(' ');

    // read cost: extract info from rest of line
    cost  = atoi(rest.substr(0, next_space).c_str());
}

// initialize sample graph (with distances of edges at 1 and fully connected)
void Graph::initialize(){
  for(int i = 0; i < edges.size(); ++i){
    node_values[i] = 1;
    nvertices++;
    for(int j = i; j < edges.size(); ++j){
      if(i == j){
    edges[i][j] = NULL; // null pointer no loops
      } else {
    edges[i][j] = new Edge(1);
    edges[j][i] = edges[i][j]; // undirected graph.
    nedges++;
      }
    }
  }
}

// returns the number of vertices in the graph
int Graph::V(){
  return nvertices;
}

// returns the number of edges in the graph
int Graph::E(){
  return nedges;
}

// tests whether there is an edge from node x to node y.
bool Graph::adjacent(const int x, const int y){
  if(x < edges.size() && y < edges[x].size()){
    if(edges[x][y] != NULL)
      return true;
  }
  return false;
}

// lists all nodes y such that there is an edge from x to y.
vector<Edge*> Graph::neighbors(const int x){
  if(x < edges.size()){
    return edges[x];
  }
  return vector<Edge*>();
}

// adds to G the edge from x to y, if it is not there.
void Graph::add(const int x, const int y){
  if(x < edges.size() && y < edges[x].size()){
    if(edges[x][y] == NULL){
      edges[x][y] = new Edge(1);
      edges[y][x] = edges[x][y]; // undirected graph
      nedges++;
    }
  }
}

// removes the edge from x to y, if it is there.
void Graph::remove(const int x, const int y){
  if(x < edges.size() && y < edges[x].size()){
    if(edges[x][y]){
      delete edges[x][y];
      edges[x][y] = NULL;
      edges[y][x] = NULL; // undirected graph
      nedges--;
    }
  }
}

// returns the value associated with the node x.
int Graph::get_node_value(const int x){
  if(0 <= x < node_values.size())
    return node_values[x];
  return 0;
}

// sets the value associated with the node x to a.
void Graph::set_node_value(const int x, const int a){
  if(0 <= x <= node_values.size() &&
     0 <= a <= node_values.size())
    node_values[a]= node_values[x];
}

// returns the value associated to the edge (x,y).
double Graph::get_edge_value(const int x, const int y){
  if(x < edges.size() && y < edges[x].size()){
    if(edges[x][y])
      return edges[x][y]->cost;
  }
  return 0;
}

// sets the value associated to the edge (x,y) to v.
void Graph::set_edge_value(const int x, const int y, Edge *v){
  if(x < edges.size() && y < edges[x].size()){
    if(edges[x][y] && v){
      v->cost = edges[x][y]->cost;
    }
  }
}

// gets a pointer to the edge associated to (x,y)
Edge* Graph::get_edge(const int x, const int y){
  if(x < edges.size() && y < edges[x].size()){
    return edges[x][y];
  }
  return NULL;
}

// print adjacency matrix
void Graph::print(){
  for(int i = 0; i < edges.size(); ++i){
    for(int j = 0; j < edges.size(); ++j){
      if(edges[i][j])
    cout << edges[i][j]->cost << "\t";
      else
    cout << "x" << "\t";
    }
    cout << endl;
  }
}