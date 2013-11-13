#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <forward_list>

using namespace std;

class Edge {
private:
    int start;
    int end;
    int value;

public:
    Edge();
    Edge(int start, int end, int value);
    int getStartPoint() {
        return start;
    }
    int getEndPoint() {
        return end;
    }
    int getValue() {
        return value;
    }
    Edge operator=(Edge&);
    bool operator==(Edge) const;
};

Edge::Edge()
{
    start = -1;
    end = -1;
    value = -1;
}

Edge::Edge(int start, int end, int value)
{
    this->start = start;
    this->end = end;
    this->value = value;
}

Edge Edge::operator=(Edge &copy)
{
    start = copy.getStartPoint();
    end = copy.getEndPoint();
    value = copy.getValue();
    return *this;
}

bool Edge::operator==(Edge check) const
{
    if (start == check.getStartPoint() &&
        end == check.getEndPoint() &&
        value == check.getValue())
    {
        return true;
    }
    else
    {
        return false;
    }
}

class Graph {

private:
    int vertices;
    vector<vector<int>> edges;
    int get_edge_value(int x, int y);
    void set_edge_value(int x, int y, int v);
    vector<int> neighbors(int x);

public:
    Graph();
    Graph(int vertices, int density, int min_val, int max_val);
    ~Graph();
    bool load_data(char *fname);
    void set_vertices(int vertices);
    int num_vertices();
    int num_edges();
    int min_span_tree(int x, vector<Edge> &min_span_set);
};

Graph::Graph() : vertices(0)
{
}

Graph::Graph(int vertices, int density, int min_val, int max_val)
{
    this->vertices = vertices;

    // initialize our vectors

    edges.resize(vertices);
    for (int i = 0; i < vertices; ++i)
        edges[i].resize(vertices);

    // we are going to ignore i == j cases since those
    // will have value 0 and not considered

    for (int i = 0; i < vertices; i++)
    {
        for (int j = i + 1; j < vertices; j++)
        {
            if (rand() % 100 < density)
            {
                int val = rand() % max_val + min_val;
                set_edge_value(i, j, val);
            }
        }
    }
}

Graph::~Graph()
{
}

void Graph::set_vertices(int vertices)
{
    this->vertices = vertices;

    // initialize our vectors

    edges.resize(vertices);
    for (int i = 0; i < vertices; ++i)
        edges[i].resize(vertices);
}

int Graph::num_vertices()
{
    return vertices;
}

int Graph::num_edges()
{
    int count = 0;
    for (int i = 0; i < vertices; i++)
    {
        for (int j = i + 1; j < vertices; j++)
        {
            if (edges[i][j] > 0)
            {
                count += 1;
            }
        }
    }

    return count;
}

int Graph::get_edge_value(int x, int y)
{
    if (x >= vertices || y >= vertices)
        return -1;
    else
        return edges[x][y];
}

void Graph::set_edge_value(int x, int y, int v)
{
    edges[x][y] = v;
    edges[y][x] = v;
}

vector<int> Graph::neighbors(int x)
{
    vector<int> result;
    for (int i = 0; i < vertices; i++)
    {
        if (edges[x][i] > 0)
        {
            result.push_back(i);
        }
    }

    return result;
}

int Graph::min_span_tree(int x, vector<Edge> &min_span_set)
{
    // get the neighbors for the given starting point
    // and add them to our list

    vector<int> set = neighbors(x);
    forward_list<Edge> check_list;

    for (int i = 0; i < static_cast<int>(set.size()); i++)
    {
        check_list.push_front(Edge(x, set[i], get_edge_value(x, set[i])));
    }

    while (1)
    {
        // if our work set has no members then we are done

        if (check_list.empty() || min_span_set.size() == vertices - 1)
            break;

        // find lowest from the set and update the min span set
        // we are using a linked list for this implementation
        // though better implementations do exist but this
        // should be sufficient for small number of vertices

        int lowest_value = -1;
        Edge index;
        for (auto it = check_list.begin(); it != check_list.end(); ++it)
        {
            if (lowest_value == -1 || it->getValue() < lowest_value)
            {
                lowest_value = it->getValue();
                index = *it;
            }
        }

        // we update our min path set with our value
        // remove it from the linked list and process
        // its neigbors

        check_list.remove(index);
        min_span_set.push_back(index);

        // process the neighbors to see if we need
        // to add more entries to our work set

        set = neighbors(index.getEndPoint());
        for (int i = 0; i < static_cast<int>(set.size()); i++)
        {
            // ignore any neighbors that point back our main point

            if (set[i] == x)
                continue;

            // if the point already exists in our min span set
            // then we need to ignore it

            bool already_exists = false;
            for (auto it = min_span_set.begin(); it != min_span_set.end(); ++it)
            {
                if (it->getEndPoint() == set[i])
                {
                    already_exists = true;
                    break;
                }
            }

            // add all the others to the work set

            if (already_exists == false)
            {
                int value = get_edge_value(index.getEndPoint(), set[i]);
                check_list.push_front(Edge(index.getEndPoint(), set[i], value));
            }
        }
    }

    // if we have Vertices - 1 edges in our min span set then
    // we have a connected graph and we'll calculate the value
    // otherwise we don't have a connected graph and will return -1
    // to indicate failure

    if (min_span_set.size() == vertices - 1)
    {
        int total = 0;
        for (auto it = min_span_set.begin(); it != min_span_set.end(); ++it)
        {
            total += it->getValue();
        }
        return total;
    }
    else
    {
        return -1;
    }
}

bool Graph::load_data(char* fname)
{
    string line;
    ifstream data_file(fname);
    if (data_file.is_open() == false)
        return false;

    // first get the number of vertices

    int value;
    getline(data_file, line);
    istringstream iss(line);
    iss >> value;
    set_vertices(value);

    // now process the edges

    while (getline(data_file, line))
    {
        int start;
        int end;
        int value;

        istringstream iss(line);
        iss >> start >> end >> value;
        set_edge_value(start, end, value);
    }

    data_file.close();
    return true;
}

int main(int argc, char **argv)
{
    Graph graph;
    vector<Edge> min_span_set;

    if (argc != 2)
    {
        cout << "Usage: min_span_tree <data_fname>" << endl;
        return EXIT_FAILURE;
    }

    if (graph.load_data(argv[1]) == false)
    {
        cout << "Unable to load data file" << endl;
        return EXIT_FAILURE;
    }

    int val = graph.min_span_tree(0, min_span_set);
    if (val != -1)
    {
        cout << "Min spanning tree value: " << val << endl;
        for (auto it = min_span_set.begin(); it != min_span_set.end(); ++it)
        {
            cout << it->getStartPoint() << " -> " << it->getEndPoint() << " : " << it->getValue() << endl;
        }

    }
    else
    {
        cout << "We don't have a connected graph" << endl;
    }

    return EXIT_SUCCESS;
}

// g++ hw3.cc -std=c++11
// ./a.out hw3_testdata.txt
/*Min spanning tree value: 30
0 -> 2 : 2
2 -> 9 : 1
9 -> 13 : 3
9 -> 12 : 3
12 -> 17 : 1
17 -> 1 : 1
1 -> 6 : 1
6 -> 5 : 1
5 -> 18 : 1
18 -> 14 : 1
14 -> 11 : 1
12 -> 11 : 1
12 -> 3 : 1
5 -> 16 : 2
16 -> 10 : 3
10 -> 15 : 2
15 -> 19 : 2
15 -> 4 : 2
4 -> 8 : 1*/