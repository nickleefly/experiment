// Copyright 2013 Steve Gribble -- gribble (at) gmail (dot) com

#include <cstdlib>
#include <iostream>

#include "./Graph.h"

// Solves HW3; the user must pass in the name of a graph file as a
// command-line argument. This program will read it into a hw3::Graph
// object, use Kruskal's algorithm to compute the MST of the graph,
// and then print out the MST to stdout.
int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "usage: ./solution graphfile.txt" << std::endl;
    return EXIT_FAILURE;
  }
  hw3::Graph<uint32_t, double> gr(argv[1]);
  hw3::Graph<uint32_t, double>::KruskalResult res = gr.ComputeMST();

  std::cout << "Cost: " << res.cost << std::endl;
  std::cout << "Edges:" << std::endl;
  for (const auto &edge : res.edges) {
    std::cout << edge.from << " " << edge.to << " " << edge.weight
              << std::endl;
  }
  return EXIT_SUCCESS;
}