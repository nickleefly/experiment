#include <cassert>
#include <chrono>
#include <iostream>
#include <ostream>
#include <random>
#include <string>
#include <vector>

const double stupid = 50.0 / 49.0;                     // Don't ask

// In Dijkstra's algorithm for each vertex we return a Link.
// The cost is the cost from the start vertex to that vertex.
// The link is the ancestor vertex in the path from the start
// vertex to the vertex. It can be used to calculate the full
// path from the start vertex.
struct Link
{
    Link() : ancestor(-1), cost(0.0) {}                 // -1 used to signal no link
    Link(int a, double c) : ancestor(a), cost(c) {}
    int ancestor;
    double cost;
};

// Helper class to represent an edge.
// Although we are dealing with an undirected graph, edges going out from
// vertex v are considered having tail = v when being part of the adjacency
// list for v.
// If the same edge connects to w, then the edge when part of the
// adjacency list for w will have tail w.
// For both lists head will be the connected vertex. The vertex at the other
// end if you will.
struct Edge
{
    Edge(int t, int h, double c) : tail(t), head(h), cost(c) {}
    int tail;
    int head;
    double cost;
};

// Range is a poor man's pair
template <typename T>
struct Range
{
    Range(T l, T u) : lower(l), upper(u) {}
    T lower;
    T upper;
};

class Graph
{
public:
    Graph(int no_vertices, double density, const Range<double>& range, unsigned long seed);

    // The number of vertices
    int V() const { return n; }

    // For each vertex return its ancestor and total cost
    std::vector<Link> Dijkstra(int start) const;

private:
    // For each vertex we have a list of incident edges.
    // Conceptionally this is the adjacency list.
    std::vector<std::vector<Edge>> edges;

    // The number of vertices
    int n;
};

Graph::Graph(int no_vertices, double density, const Range<double>& range, unsigned long seed)
    : edges(no_vertices), n(no_vertices)
{
    // Construct a random number engine
    std::mt19937 mt(seed != 0 ? seed : 1UL);
    // Construct a random number generator having uniform distribution [0.0, 1.0)
    std::uniform_real_distribution<double> arbiter;
    // Construct another random number generator having a uniform distribution [lower, upper)
    std::uniform_real_distribution<double> cost_gen(range.lower, range.upper);

    for (int i = 0; i != no_vertices; ++i)
    {
        for (int j = i + 1; j != no_vertices; ++j)
        {
            double x = arbiter(mt);
            if (x >= density)
            {
                continue;           // Skip this edge
            }

            double cost = cost_gen(mt);

            edges[i].push_back(Edge(i, j, cost));
            edges[j].push_back(Edge(j, i, cost));
        }
    }
}

std::vector<Link> Graph::Dijkstra(int start) const
{
    std::vector<Link> scores(V());                  // What the function returns

    // S is the set of vertices for which we have already determined the shortest path
    // T is the set of vertices one hoop away from a vertex in S
    std::vector<bool> S(V());                         // Chosen set
    std::vector<int> T;                               // Set of candidates
    T.reserve(V());                                   // Make some room

    // Initialize with the start vertex
    scores[start] = Link(0, 0.0);
    T.push_back(start);

    while (!T.empty())
    {
        // Find the element in T having the smallest score
        int index = 0;
        for (std::vector<int>::size_type i = 1; i != T.size(); ++i)
        {
            if (scores[T[i]].cost < scores[T[index]].cost)
            {
                index = i;
            }
        }

        // Copy the last element into the hole we were about to make
        std::swap(T[index], T.back());     // I should have included <utility>
        const int tail = T.back();
        T.pop_back();

        assert(!S[tail]);                  // Sanity check
        S[tail] = true;

        // Get the adjacency list for this vertex
        const std::vector<Edge>& incident_edges = edges[tail];

        // Traverse the list
        for (std::vector<Edge>::size_type i = 0; i != incident_edges.size(); ++i)
        {
            const Edge& edge = incident_edges[i];
            const int head = edge.head;
            const double cost = edge.cost;

            // If the final cost has already been determined, nothing to do
            if (S[head])
            {
                continue;
            }

            const double new_cost = scores[tail].cost + cost;

            // Check if this is the first time we see this vertex
            if (scores[head].ancestor == -1)
            {
                T.push_back(head);
                scores[head] = Link(tail, new_cost);
            }
            else if (new_cost < scores[head].cost)
            {
                scores[head] = Link(tail, new_cost);
            }
        }
    }

    return scores;
}

double single_run(const Graph& graph, int start)
{
    const std::vector<Link> costs = graph.Dijkstra(start);

    int count = 0;
    double accumulate = 0.0;

    for (std::vector<Link>::size_type i = 0; i != costs.size(); ++i)
    {
        const Link& link = costs[i];
        const int ancestor = link.ancestor;
        const double cost = link.cost;

        if (ancestor != -1)
        {
            accumulate += cost;
            ++count;
        }
    }

    return accumulate / count;
}

int main(int argc, char* argv[])
{
    // Configuration
    const int no_vertices = 50;
    const double density = 0.20;
    const Range<double> range(1.0, 10.0);

    // If no option is specified, use -m
    const std::string option = argc == 1 ? "-m" : argv[1];

    if (option == "-m")
    {
        // Get random seed
        std::random_device rd;
        const unsigned long seed = rd();

        // Print out the seed for debugging purpose
        std::cout << "Seed = " << seed << std::endl;

        // If no iteration count is specified, default to 1
        const int no_iterations = (argc == 1 || argc == 2) ? 1 : std::stoi(argv[2]);

#ifdef NDEBUG
        const std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
#endif

        double accumulator = 0.0;
        for (int i = 0; i != no_iterations; ++i)
        {
            // Create the random graph
            const Graph graph(no_vertices, density, range, seed + i);

            const int vertex = 0;             // Start vertex
            const double average = single_run(graph, vertex);

            accumulator += average;
        }

#ifdef NDEBUG
        const std::chrono::time_point<std::chrono::system_clock> stop = std::chrono::system_clock::now();
        const std::chrono::duration<double> duration = stop - start;
        const double total = duration.count();
        const double individual = total / no_iterations;
#endif

        std:: cout << "\nAverage path length " << stupid * (accumulator / no_iterations) << std::endl;

#ifdef NDEBUG
        std::cout << "Total time for " << no_iterations << " iterations = " << total << " sec" << std::endl;
        std::cout << "Time per iterations = " << 1000.0 * individual << " msec" << std::endl;
#endif
    }
    else
    {
        std::cout << "Usage: HW2.exe [-m [count]]" << std::endl;
        std::cout << "For -m, generate count random graphs. The default is 1." << std::endl;
    }

    std::cout << std::endl;

    return 0;
}

//g++ -DNDEBUG -std=c++11 -Wall -pedantic -W -Wextra -O3 -o HW2.exe HW2.cpp