#include <iostream>
#include <vector>
#include "Graph.h"
#include <fstream>

class MST {
public:
    MST();
    MST(ifstream &input); // from file
    MST(int numNodes, int distanceRange); // random graph
    ~MST();
    void computeMST(); // Prim's MST
    void mstEdges(); // print edges and total weight
private:
    Graph G;
    int totalWeight;
    vector<bool> marked; // keep track of vertices in MST
    vector<int> mstVertices; //order of vertices added to MST
    unsigned numVertices;

};