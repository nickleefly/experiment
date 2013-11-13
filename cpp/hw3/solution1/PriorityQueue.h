#include <vector>
using namespace std;

// PQueue - minheap
// header and implementation in same file to make the linker see the instantiated templates

template <typename T, typename PriorityT>
class PriorityQueue {
public:
    class Node {
        friend PriorityQueue;
        T node;
        PriorityT priority;
    };

    PriorityQueue();
    ~PriorityQueue();
    void chgPriority(T, PriorityT); // chg prioirty
    bool contains(T);
    inline int size() { return nodes.size(); }
    void insert(T, PriorityT);  // insert element
    T minPriority(); // removes and returns;
    inline T top() { return nodes[0]->node; }; // just returns min prioirty element
private:
    vector<Node *> nodes;
    void sink(Node *n);
    void swim(Node *n, int pos);


};

template<typename T, typename PriorityT>
PriorityQueue<typename T, typename PriorityT>::PriorityQueue()
{}

template<typename T, typename PriorityT>
PriorityQueue<typename T, typename PriorityT>::~PriorityQueue()
{}

// resstore heap property by sinking the elemenent with high priority by comparing with it's children
// O(logn)
template<typename T, typename PriorityT>
void PriorityQueue<typename T, typename PriorityT>::sink(Node* n){
    int pos = 0;
    while (pos < static_cast<signed>(nodes.size() / 2)) {
        if ((nodes[pos]->priority >  nodes[2 * pos]->priority) || (nodes[pos]->priority > nodes[2 * pos + 1]->priority)) {
            Node *tmp = nodes[pos];
            if (nodes[2 * pos]->priority < nodes[2 * pos + 1]->priority) {
                nodes[pos] = nodes[2 * pos];
                nodes[2 * pos] = tmp;
                pos = 2 * pos;
            }
            else {
                nodes[pos] = nodes[2 * pos + 1];
                nodes[2 * pos + 1] = tmp;
                pos = 2 * pos + 1;
            }
        }
        else
            break;
    }
}

// swim up till the new element find's it correct place
// O(logn)
template<typename T, typename PriorityT>
void PriorityQueue<typename T, typename PriorityT>::swim(Node *n, int pos){
    while (pos > 0) {
        int parentPos;
        if (pos % 2 == 0) {
            parentPos = (pos - 1) / 2;
        }
        else {
            parentPos = pos / 2;
        }
        if (nodes[parentPos]->priority < nodes[pos]->priority)
            break;
        Node *tmp = nodes[parentPos];
        nodes[parentPos] = nodes[pos];
        nodes[pos] = tmp;
        pos = parentPos;
    }
}

// check if element in PQ
template<typename T, typename PriorityT>
bool PriorityQueue<typename T, typename PriorityT>::contains(T n) {
    for (unsigned int i = 0; i < nodes.size(); i++) {
        if (nodes[i]->node == n)
            return true;
    }
    return false;
}

// change the priority of an element
// then swim up
// O(logn)
template  <typename T, typename PriorityT>
void PriorityQueue<typename T, typename PriorityT>::chgPriority(T n, PriorityT priority) {
    for (unsigned int i = 0; i < nodes.size(); i++) {
        if (nodes[i]->node == n) {
            nodes[i]->priority = priority;
            swim(nodes[i], i);
            break;
        }

    }

}
// insert at the end and swim upto find it's rightful place
// O(logn)
template  <typename T, typename PriorityT>
void PriorityQueue<typename T, typename PriorityT>::insert(T n, PriorityT priority) {
    Node *newNode = new Node;
    newNode->node = n;
    newNode->priority = priority;
    nodes.push_back(newNode);
    swim(newNode, nodes.size() - 1);
}

// remove the min priority element
// put the last element first and then sink it to find the rightful place
// O(logn)
template  <typename T, typename PriorityT>
T PriorityQueue<typename T, typename PriorityT>::minPriority() {
    Node *n = nodes[0];
    T nodeNum = n->node;
    int size = nodes.size();
    nodes[0] = nodes[--size];
    nodes.pop_back();
    if (size > 1)
        sink(nodes[0]);
    return nodeNum;
}