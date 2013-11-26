// Copyright 2013 Steve Gribble -- gribble (at) gmail (dot) com

#include <cstdlib>
#include <iostream>
#include <map>

#include "./Graph.h"
#include "./HexBoard.h"

namespace hw4 {

HexBoard::HexBoard(uint16_t width, uint16_t height)
  : width_(width), height_(height) {
  // Sanity check the arguments.
  if ((width_ == 0) || (height_ == 0)) {
    std::cerr << "HexBoard() constructor called with zero width or height."
              << std::endl;
    exit(EXIT_FAILURE);
  }

  // Add the graph nodes.
  for (uint16_t row = 0; row < height_; ++row) {
    for (uint16_t col = 0; col < width_; ++col) {
      if (graph_.AddNode(EMPTY) != (row * width_) + col) {
        std::cerr << "graph_.AddNode() returned unexpected Node identifier."
                  << std::endl;
        exit(EXIT_FAILURE);
      }
    }
  }
}

HexBoard::occupancy HexBoard::GetOccupancy(uint16_t x, uint16_t y) const {
  // Sanity check the arguments.
  if ((x >= width_) || (y >= height_)) {
    std::cerr << "GetOccupancy() called with position beyond board edges."
              << std::endl;
    exit(EXIT_FAILURE);
  }
  return graph_.get_node_value(x + (y * width_));
}

bool HexBoard::SetOccupancy(uint16_t x, uint16_t y,
                            HexBoard::occupancy color) {
  // Sanity check the arguments.
  if ((x >= width_) || (y >= height_)) {
    std::cerr << "SetOccupancy() called with position beyond board edges."
              << std::endl;
    exit(EXIT_FAILURE);
  }
  if (color == EMPTY) {
    std::cerr << "SetOccupancy() called with EMPTY as color."
              << std::endl;
    exit(EXIT_FAILURE);
  }

  // See if the square is already taken.
  if (GetOccupancy(x, y) != EMPTY) {
    return false;
  }

  // Set the graph to the new occupancy.
  graph_.set_node_value(x + (y * width_), color);

  // Set the connections to neighboring cells of the same occupancy.
  // First to left edge.
  if (x != 0) {
    if (GetOccupancy(x - 1, y) == color)
      graph_.AddEdge((x - 1) + (y * width_), x + (y * width_), 1.0);
  }

  // To right edge.
  if (x != width_ - 1) {
    if (GetOccupancy(x + 1, y) == color)
      graph_.AddEdge(x + (y * width_), (x + 1) + (y * width_), 1.0);
  }

  // To upper-left.
  if (y != 0) {
    if (GetOccupancy(x, y - 1) == color)
      graph_.AddEdge(x + ((y - 1) * width_), x + (y * width_), 1.0);
  }

  // To upper-right.
  if ((x != (width_ - 1)) && (y != 0)) {
    if (GetOccupancy(x + 1, y - 1) == color)
      graph_.AddEdge((x + 1) + ((y - 1) * width_), x + (y * width_), 1.0);
  }

  // To bottom-left.
  if ((x != 0) && (y != height_ - 1)) {
    if (GetOccupancy(x - 1, y + 1) == color)
      graph_.AddEdge(x + (y * width_), (x - 1) + ((y + 1) * width_), 1.0);
  }

  // To bottom-right.
  if (y != height_ - 1) {
    if (GetOccupancy(x, y + 1) == color)
      graph_.AddEdge(x + (y * width_), x + ((y + 1) * width_), 1.0);
  }

  return true;
}

const Graph<HexBoard::occupancy, double>& HexBoard::GetGraph() const {
  return graph_;
}

bool HexBoard::HasBlueWon() const {
  ShortestPath<occupancy> sp(graph_);
  for (uint16_t lrow = 0; lrow < height_; ++lrow) {
    for (uint16_t rrow = 0; rrow < height_; ++rrow) {
      ShortestPath<occupancy>::Path path;
      if (sp.get_shortest_path(lrow * width_, rrow * width_ + (width_ - 1),
                               &path)) {
        if (path.get_node(0).value == BLUE) {
          return true;
        }
      }
    }
  }
  return false;
}

bool HexBoard::HasRedWon() const {
  ShortestPath<occupancy> sp(graph_);
  for (uint16_t tcol = 0; tcol < width_; ++tcol) {
    for (uint16_t bcol = 0; bcol < width_; ++bcol) {
      ShortestPath<occupancy>::Path path;
      if (sp.get_shortest_path(tcol, (height_ - 1) * width_ + bcol, &path)) {
        if (path.get_node(0).value == RED) {
          return true;
        }
      }
    }
  }
  return false;
}

std::string HexBoard::ToString() const {
  static std::string kBorder = "◼", kEmpty = "•", kBlue = "B", kRed = "R";
  static std::map<occupancy, std::string> char_map = {{EMPTY, kEmpty},
                                                      {BLUE, kBlue},
                                                      {RED, kRed}};
  static std::map<bool, std::string> horiz_map = {{true, "-"}, {false, " "}};
  static std::map<bool, std::string> lrd_map = {{true, "\\"}, {false, " "}};
  static std::map<bool, std::string> rld_map = {{true, "/"}, {false, " "}};
  std::string retstr;

  // Do the top border.
  for (uint32_t offset = 0; offset < (width_ - 1) * 4 + 5; ++offset) {
    retstr += kBorder;
  }
  retstr += '\n';

  // Do the rows.
  for (uint16_t row = 0; row < height_; ++row) {
    // Do a horizontal row. First, place the border.
    retstr += " ";
    for (uint16_t i = 0; i < row; ++i) retstr += "  ";
    retstr += kBorder + " ";
    // Next, place the pieces and horizontal edges.
    for (uint16_t col = 0; col < width_; ++col) {
      retstr += char_map[GetOccupancy(col, row)];
      if (col != width_ - 1) {
        retstr += " ";
        retstr += horiz_map[graph_.AreNodesAdjacent(row * width_ + col,
                                                    row * width_ + col + 1)];
        retstr += " ";
      }
    }
    // Finish up with the border.
    retstr += " " + kBorder + "\n";

    if (row != height_ - 1) {
      // Do a diagonal edge row. First, place the border.
      retstr += "  ";
      for (uint16_t i = 0; i < row; ++i) retstr += "  ";
      retstr += kBorder + " ";
      for (uint16_t col = 0; col < width_; ++col) {
        // Next, place the diagonal edges.
        retstr += lrd_map[graph_.AreNodesAdjacent(row * width_ + col,
                                                  (row + 1) * width_ + col)];
        if (col != width_ - 1) {
          retstr += " ";
          retstr += rld_map[graph_.AreNodesAdjacent(row * width_ + col + 1,
                                                    (row + 1) * width_ + col)];
          retstr += " ";
        }
      }
      // Finish up with the border.
      retstr += " " + kBorder + "\n";
    }
  }

  // Do the final border.
  for (uint32_t offset = 0; offset < height_ * 2; offset++) {
    retstr += " ";
  }
  for (uint32_t offset = 0; offset < (width_ - 1) * 4 + 5; ++offset) {
    retstr += kBorder;
  }
  retstr += '\n';

  return retstr;
}

} // end namespace hw4