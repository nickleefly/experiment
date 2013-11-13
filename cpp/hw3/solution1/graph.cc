#include <iostream>
#include <fstream>
#include <random>
#include "Graph.h"
#include <chrono>
using namespace std;

Graph::Graph(int numVertices, int numEdges) :vertices(numVertices),
edges(numEdges), distances(numVertices, INFINITY), adjList(numVertices)
{}

//read a graph from given file
Graph::Graph(int numNodes, int numEdges, int distanceRange) :vertices(numNodes), edges(numEdges), distances(numNodes, INFINITY), adjList(numNodes) {
    typedef std::chrono::high_resolution_clock myclock;
    static default_random_engine e;
    e.seed(static_cast<unsigned long>(time(0)));
    uniform_int_distribution<unsigned> u(1, distanceRange);
    for (int i = 0; i < vertices; i++) {
        for (int j = 0; j < vertices; j++) {
            if (i == j) continue;
            add_edge_and_set_value(i, j, u(e));
        }
    }
}
// random Graph Generation
Graph::Graph(ifstream &in) : vertices(0),
edges(0), distances(0, INFINITY), adjList(0) {
    in >> vertices;
    edges = 0;
    distances.resize(vertices);
    for (int i = 0; i < vertices; i++) {
        distances.push_back(INFINITY);
    }
    adjList.resize(vertices);
    while (!in.eof()) {
        int source;
        int dest;
        int weight;
        in >> source;
        in >> dest;
        in >> weight;
        addEdge(source, dest);
        set_edge_value(source, dest, weight);
        edges++;
    }
    in.close();
}
Graph::~Graph()
{
    for (auto it = adjList.begin(); it != adjList.end(); ++it)
    for (auto it2 = (*it).begin(); it2 != (*it).end(); ++it2)
        delete *it2;
}

unsigned Graph::V() {
    return vertices;
}
unsigned Graph::E() {
    return edges;
}

// check if two nodes are adjacent
bool Graph::adjacent(int x, int y) {
    vector<Edge*> vec = adjList[x];
    for (auto it = vec.begin(); it != vec.end(); it++) {
        if ((*it)->getDest() == y)
            return true;
    }
    return false;
}

// return all neighbors for a node
const vector<int> Graph::neighbors(int x) {
    vector<Edge*> vec = adjList[x];
    vector<int> neighb;
    for (auto it = vec.begin(); it != vec.end(); it++) {
        neighb.push_back((*it)->getDest());
    }
    return neighb;
}

const vector<Edge *>& Graph::adjacent_edges(int x){
    return adjList[x];
}

void Graph::addEdge(int x, int y) {
    Edge *edge = new Edge(x, y);
    adjList[x].push_back(edge);
    edges++;

}
//create a edge and set weight
void Graph::add_edge_and_set_value(int x, int y, int weight) {
    Edge *edge = new Edge(x, y);
    edge->setWeight(weight);
    adjList[x].push_back(edge);

    edges++;
}

// find a edge and delete it
// simple vector traversal
void Graph::deleteEdge(int x, int y) {
    int size = adjList[x].size();
    if (size  > 0) {
        vector<Edge*> vec = adjList[x];
        auto it = vec.begin();
        while (it != vec.end()) {
            if ((*it)->getDest() != y) {
                ++it;
            }
            else {
                vec.erase(it);
                break;
            }
        }
    }
}

double Graph::get_node_value(int x) {
    return distances[x];
}

void Graph::set_node_value(int x, double a) {
    distances[x] = a;
}

// get the edge weight
int Graph::get_edge_value(int x, int y) {
    int size = adjList[x].size();
    if (size > 0) {
        vector<Edge*> vec = adjList[x];
        for (auto it = vec.begin(); it != vec.end(); ++it) {
            if ((*it)->getDest() == y)   {
                int w = (*it)->getWeight();
                return w;
            }
        }
    }
    return -1;
}


// edge list traversal and set weight
void Graph::set_edge_value(int x, int y, int w) {
    int size = adjList[x].size();
    if (size > 0) {
        vector<Edge*> vec = adjList[x];
        for (auto it = vec.begin(); it != vec.end(); ++it) {
            if ((*it)->getDest() == y)   {
                (*it)->setWeight(w);
                break;
            }
        }
    }
}
