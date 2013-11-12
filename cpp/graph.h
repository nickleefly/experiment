#include <vector>
#include <string>
#include "Edge.h"
#include <fstream>

using namespace std;


class Graph {
public:
    Graph(int numVertices = 0, int numEdges = 0);
    Graph(ifstream &in);
    Graph(int numVertices, int numEdges, int  distanceRange);
    ~Graph();
    unsigned V();
    unsigned E();
    bool adjacent(int x, int y); // is x and y adjacent?
    const vector<int> neighbors(int x); // neighbors of x
    void addEdge(int x, int y);
    void add_edge_and_set_value(int x, int y, int weight);
    void deleteEdge(int x, int y);
    double get_node_value(int x); // set the value at a node
    void set_node_value(int x, double a);
    int get_edge_value(int x, int y);
    void set_edge_value(int x, int y, int w);
    const vector<Edge *>& adjacent_edges(int x);

private:
    int vertices;
    vector<vector<Edge*>> adjList; // adjaceny list for each node
    int edges;
    vector<double> distances; // values for each node from source
};
