gameplay.cc -- https://gist.github.com/gribble/c1da1cbcd89084d0fe41#file-gameplay-cc -- the actual game playing program itself, including main(). This contains logic for prompting the user for moves, displaying messages when a user wins, and so on.

HexBoard.h -- https://gist.github.com/gribble/c1da1cbcd89084d0fe41#file-hexboard-h -- the interface to my HexBoard class, which includes functionality for constructing a Hex board, modifying the occupancy of cells, testing to see whether a player has won the game, and producing a string representation of the current board layout and state.

HexBoard.cc -- https://gist.github.com/gribble/c1da1cbcd89084d0fe41#file-hexboard-cc -- the implementation of the HexBoard class

Graph.h -- https://gist.github.com/gribble/c1da1cbcd89084d0fe41#file-graph-h -- my Graph class from HW2/HW3

ShortestPath.h -- https://gist.github.com/gribble/c1da1cbcd89084d0fe41#file-shortestpath-h -- my ShortestPath Dijkstra implementation from HW2.

UnionFindForest.h -- https://gist.github.com/gribble/c1da1cbcd89084d0fe41#file-unionfindforest-h -- the interface to my UnionFindForest class, which is used by Kruskal's algorithm in Graph.h's Graph::ComputeMST().

UnionFindForest.cc -- https://gist.github.com/gribble/c1da1cbcd89084d0fe41#file-unionfindforest-cc -- the implementation of my UnionFindForest class.

Note that my code uses features from C++11. So, if you want to try compiling it, you'll need access to a recent compiler with support for those features. On a Mac, you'll need to install the latest Xcode, and use the following command line to compile:

  clang++ -Wall -std=c++11 -stdlib=libc++ -o gameplay gameplay.cc HexBoard.cc UnionFindForest.cc