#include <iostream>
#include <vector>
#include <queue>


using std::cout;
using std::endl;
using std::vector;
using std::priority_queue;


class Vertice; // Forward declaration
class Exception {};


class Edge {
    private:
        unsigned int _cost;
        Vertice* _a;
        Vertice* _b;

    public:
        Edge(Vertice* a, Vertice* b, unsigned int cost) : _a(a), _b(b), _cost(cost) {};

        unsigned int getCost() const {
            return _cost;
        }

        Vertice* getNext() {
            return _b;
        }
};


class Vertice {
    private:
        unsigned int _id;
        bool _visited;
        vector<Edge*> _edges;

    public:
        Vertice(unsigned int id) : _id(id), _visited(false) {};

        bool isVisited() const {
            return _visited;
        }

        void setVisited() {
            _visited = true;
        }

        unsigned int getId() const {
            return _id;
        }

        vector<Edge*> getEdges() {
            return _edges;
        }

        void addEdge(Edge* edge) {
            _edges.push_back(edge);
        }
};


class Path {
    private:
        Vertice* _current;
        vector<Vertice*> _steps;
        unsigned int _distance;

    public:
        Path(Vertice *vertice) : _current(vertice), _distance(0) {};

        Path(Path *path, Edge *edge) {
            _current = edge->getNext();
            _distance = path->getDistance() + edge->getCost();
            _steps.push_back(path->getCurrent());
        }

        unsigned int getDistance() const {
            return _distance;
        }

        Vertice* getCurrent() const {
            return _current;
        }

        vector<Vertice*> getSteps() const {
            return _steps;
        }
};

bool operator<(const Path &a, const Path &b) {
    return a.getDistance() > b.getDistance();
}


class Graph {
    private:
        vector<Vertice*> _vertices;

        Path* getShortestPath(Vertice* start, Vertice* finish) {
            priority_queue<Path*> queue;

            queue.push(new Path(start));

            while ( !queue.empty() ) {
                Path* path = queue.top();
                queue.pop();

                Vertice* current = path->getCurrent();

                if ( current == finish ) {
                    return path;
                }
                for ( auto &edge : current->getEdges() ) { // Requires "-std=c++0x" parameter
                    if ( !edge->getNext()->isVisited() ) {
                        queue.push(new Path(path, edge));
                        edge->getNext()->setVisited();
                    }
                }
                delete path;
            }
            throw Exception(); // Path doesn't exist
        }

    public:
        Graph() {};

        void addVertice(Vertice* vertice) {
            _vertices.push_back(vertice);
        }

        vector<Vertice*> getPath(Vertice* start, Vertice* finish) {
            return getShortestPath(start, finish)->getSteps();
        }

        unsigned int getPathSize(Vertice* start, Vertice* finish) {
            return getShortestPath(start, finish)->getDistance();
        }
};


int main() {
    Graph* graph = new Graph();
    Vertice* v1 = new Vertice(1);
    Vertice* v2 = new Vertice(2);
    Vertice* v3 = new Vertice(3);
    Vertice* v4 = new Vertice(4);
    Vertice* v5 = new Vertice(5);
    Vertice* v6 = new Vertice(6);
    Vertice* v7 = new Vertice(7);
    Edge* e1t2 = new Edge(v1, v2, 5);
    Edge* e1t3 = new Edge(v1, v3, 6);
    Edge* e1t4 = new Edge(v1, v4, 7);
    Edge* e2t5 = new Edge(v2, v5, 3);
    Edge* e4t6 = new Edge(v4, v6, 2);
    Edge* e3t6 = new Edge(v3, v6, 4);
    Edge* e5t6 = new Edge(v5, v6, 1);
    Edge* e5t7 = new Edge(v5, v7, 4);
    Edge* e6t7 = new Edge(v6, v7, 3);
    v1->addEdge(e1t2);
    v1->addEdge(e1t4);
    v1->addEdge(e1t3);
    v2->addEdge(e2t5);
    v3->addEdge(e3t6);
    v4->addEdge(e4t6);
    v5->addEdge(e5t6);
    v5->addEdge(e5t7);
    v6->addEdge(e6t7);

    cout << graph->getPathSize(v1, v7) << endl;

    return 0;
}