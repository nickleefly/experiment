#include "Edge.h"
using namespace std;

Edge::Edge(int from, int to, int w) : source(from), dest(to), weight(w)

{}

Edge::~Edge()
{}

ostream& operator<<(ostream &os, const Edge &e) {
    os << e.source << "--->" << e.dest << endl;
    return os;
}