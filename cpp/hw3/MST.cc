#include "MST.h"
#include "PriorityQueue.h"
#include <string>
#include <fstream>


MST::MST()
{}
MST::MST(ifstream &in) :totalWeight(0), G(in), marked(G.V(), false), mstVertices(G.V()), numVertices(0)
{}
MST::MST(int numNodes, int distanceRange) : totalWeight(0), G(numNodes, 0, distanceRange),marked(G.V(), false), mstVertices(G.V()), numVertices(0)
{}
MST::~MST()
{}

// Print edges and total weight
void MST::mstEdges() {
    for (unsigned i = 0; i < G.V()-1; i++) {
        cout << mstVertices[i] << "---->" << mstVertices[i + 1] << " " << G.get_edge_value(mstVertices[i], mstVertices[i+1]) << endl;
        totalWeight += G.get_edge_value(mstVertices[i], mstVertices[i + 1]);
    }
    cout << "Total Weight is " << totalWeight;
}

// Prim's MST
void MST::computeMST() {

    int currentSource = 0;
    mstVertices[numVertices++] = currentSource;
    marked[currentSource] = true;
    PriorityQueue<Edge*, double> pq;

    while (numVertices < G.V()) {
        const vector<Edge *>& edges = G.adjacent_edges(currentSource);
        for (auto it = edges.begin(); it != edges.end(); ++it)
            pq.insert(*it, (*it)->getWeight());
        while (true && pq.size() > 0) {
            Edge* nextEdge = pq.minPriority();
            if (marked[nextEdge->getSource()] && marked[nextEdge->getDest()]) continue;
            if (marked[nextEdge->getSource()]) {
                currentSource = nextEdge->getDest();
                break;
            }
            else {
                currentSource = nextEdge->getSource();
                break;
            }
        }

        mstVertices[numVertices++] = currentSource;
        marked[currentSource] = true;
        //cout << numVertices << endl;
    }

}


// read either from given file or generate a random graph of n nodes with a distance range
int main(void) {
    string filename;
    int numNodes;
    int distanceRange;
    enum Type { FILE, RANDOM, ERROR };
    Type type = ERROR;
    while (type != FILE && type != RANDOM) {
        unsigned num;
        cout << "Input 0 for reading from file: " << endl;
        cout << "Input 1 for random graph generation: " << endl;
        cin >> num;
        type = static_cast<Type>(num);
    }
    if (type == FILE) {
        ifstream input;
        while (true) {
            cout << "Input filename" << endl;
            cin >> filename;
            input.open(filename, ios::in);
            if (!input.fail()) break;
            input.clear();
        }
        MST mst(input);
        mst.computeMST();
        mst.mstEdges();
    }
    else {
        cout << "input num nodes: ";
        cin >> numNodes;
        cout << "input distance Range: ";
        cin >> distanceRange;

        MST mst(numNodes, distanceRange);
        mst.computeMST();
        mst.mstEdges();
    }
    cin.get();
    cin.get();
    return 0;
}