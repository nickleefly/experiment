#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <ctime>
#include <cstdlib>
#include <fstream>

const int DEBUG = 0;

// Homework Inputs

const int GSIZE = 50;
const float GRAPH_DENSITY = 0.4;

// Global variable declarations

    double ** grfLen;
    bool ** grf;

using namespace std;

double prob() {
    // REFERENCE: Lecture video 4.14
    return (static_cast<double>(rand())/RAND_MAX);
}

// Djikstra shortest path algorithm

class Dijkstra
{

    private:
        int nNodes;
        int nEdges;
        double dist[GSIZE];
        bool visited[GSIZE];
        int previous[GSIZE];
        int Q[GSIZE];
        int iQ;
    public:
        int shortestPath  (int source, int destination);
        void showVisited ();
        void showdist();
        void showQ();


};
void Dijkstra::showVisited(){ // printout visited array for debugging
    if (DEBUG==0) return;
    for (int i=0;i<GSIZE;i++) {
        cout<<"visited["<<i<<"]="<<visited[i]<<endl;
    }
}
void Dijkstra::showQ() { // printout Q array for debugging
    if (DEBUG==0) return;
    for (int i=0;i<iQ;i++)
        cout<<"Q["<<i<<"]="<<Q[i]<<endl;
}
void Dijkstra::showdist() { // printout dist array for debugging
    if (DEBUG==0) return;
    for (int i=0;i<GSIZE;i++) {
        cout<<"dist["<<i<<"]="<<dist[i]<<endl;
    }
}
int Dijkstra::shortestPath  (int source, int destination) {

// as described in http://en.wikipedia.org/wiki/Dijkstra's_algorithm

int u;
// initilization
if (DEBUG) cout<<"1 - INITIALIZATION"<<endl;
iQ = 0;
for (int i=0;i<GSIZE;i++) {
    dist[i] = 1000000;
    visited[i] = false;
    previous[i] = -1; // undefined
}
dist[source] = 0;
Q[iQ] = source;
iQ++;
showQ();
//cout<<"iQ:" <<iQ<<endl;
//cout<<"Q:" <<Q[1]<<endl;
if (DEBUG) cout<<"2 - START LOOP"<<endl;
while (iQ!=0) {
    // Q is not empty
    if (DEBUG) cout<<"Q is not empty"<<endl;

    if (DEBUG) cout<<"3 - Copy dist to tmpDist"<<endl;
    float tmpDist[iQ];
    for (int i=0;i<iQ;i++) {
        tmpDist[i] = dist[Q[i]];
        //cout<<"tmpDist:" <<tmpDist[0]<<endl;
    }
    if (DEBUG) cout<<"4 - Find vertex in Q with the smallest distance in dist[] and has not been visited"<<endl;
    float smallest = tmpDist[0];
    int k=0;
    while (k<iQ) {
         if ( tmpDist[k]<=smallest && !visited[Q[k]]) {
            //cout<<"tmpDist:" <<tmpDist[0]<<endl;
            smallest = tmpDist[k];
            u = Q[k];
            if (DEBUG) {cout<<"u=" <<u<<endl;}
        }
        k++;

    }
     //cout<<"smallest:" <<smallest<<endl;
    showQ();
    if (DEBUG) cout<<"uFinal=" <<u<<endl;
    if (u==destination) {
        cout<<"destination reached="<<u<<" Shortest distance from source="<<dist[u]<<endl;

        return(dist[u]);

    }

    // remove u from Q
    // first copy to a temporary array
    if (DEBUG) cout<<"5 - Remove u from Q and add it to visited"<<endl;
    float tmpQ[iQ];
    for (int i=0; i<iQ;i++ ) {
        tmpQ[i] = Q[i];
    }

    // recreat Q array with u removed
    int j = 0;
    for (int i=0;i<iQ;i++) {
        if (tmpQ[i] != u) {
            Q[j] = tmpQ[i];
            j++;
        }
    }
    iQ--;
    if (DEBUG) cout<<"Removed node "<<u<<" from Q"<<endl;
    showQ();
    if (DEBUG) cout<<"Added node "<<u<<" to visited"<<endl;
    visited[u] = true;
    showVisited();

    // process neighbors (use grfLen)
    if (DEBUG) cout<<"6 - Processing neighbors"<<endl;
    if (DEBUG) {
    for (int i=0;i<GSIZE;i++)
        if (grfLen[i][j] != 0)
            cout<<"Original: grfLen["<<i<<"]["<<j<<"]"<<grfLen[i][j]<<endl;
    }

    if (DEBUG) cout<<"7 - Accumulate shortest distance from source"<<endl;
    for (int i=0;i<GSIZE;i++) {

        if (grfLen[i][u] != 0.0) {
            double alt = dist[u]+grfLen[i][u];
            if (DEBUG) cout<<"i:"<<i<<" alt:="<<alt<<endl;
            if (alt<=dist[i] && !visited[i]) {
                if (DEBUG) cout<<"alt="<<alt<<" is less than dist["<<i<<"]="<<dist[i]<<endl;
                dist[i] = alt;
                previous[i] = u;
                Q[iQ]=i;
                iQ++;
                showQ();
                showdist();
            }
        }

    }

}
return(0);
}


int main()
{
    //double ** grfLen;
    //bool ** grf;
    Dijkstra d;

    srand(time(0)); // seed rand()
    grf = new bool*[GSIZE];
    grfLen = new double*[GSIZE];
    for (int i=0;i<GSIZE;i++) {
        grf [i] = new bool[GSIZE];
        grfLen[i] = new double[GSIZE];
    }
    int actEdges = 0;
    for (int i=0;i<GSIZE;i++)
    {
        for (int j=i;j<GSIZE;j++) {
            if (i==j) {
                grf[i][j] = false;
                grfLen[i][j] = 0.0;
            }
            else {
                grf[i][j] = grf[j][i] = (prob()<GRAPH_DENSITY);
                if (grf[i][j] == 1) {
                    grfLen[i][j] = grfLen[j][i] = float(rand() % 10 + 1);
                    actEdges++;
                }
                //cout<<"i="<<i<<" j="<<j<<" edge="<<grf[i][j]<<" length="<<grfLen[i][j]<<endl;
                }
            }
    }
    cout<<"actual Edges: "<<actEdges*2<<endl;

    for (int i=0;i<GSIZE;i++)
        for (int j=i;j<GSIZE;j++)
            cout<<"grfLen["<<i<<"]["<<j<<"]="<<grfLen[i][j]<<endl;

    // calculate shortest paths and average
    int pathLength[GSIZE];
    for (int i=0;i<GSIZE;i++) {
        pathLength [i] = d.shortestPath(0, i);
        cout<<"i="<<i<<" len="<<pathLength [i]<<endl;
    }
    // average
    int k=0;
    double total = 0.0;
    for (int i=0;i<GSIZE;i++) {
        if (pathLength[i] != 0) {
            // cout<<pathLength[i]<<endl;
            total = total + float(pathLength[i]);
            k++;
        }
    }
    double path_average = total/float(k);
    cout << "Average: "<<path_average<<endl;

    return 0;
}
