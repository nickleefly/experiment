// Edge class to store weights/soure/destination
#include <iostream>
using namespace std;


class Edge {
    friend ostream& operator<<(ostream&, const Edge&);
public:
    Edge(int x, int y, int w = 0);
    ~Edge();
    inline int getDest() const { return dest; }
    inline int getSource() const { return source; }
    inline int getWeight() const { return weight; }
    inline void setWeight(int w) { weight = w; }

private:
    int source;
    int dest;
    int weight;

};