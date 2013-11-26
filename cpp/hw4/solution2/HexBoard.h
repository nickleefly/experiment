// Copyright 2013 Steve Gribble -- gribble (at) gmail (dot) com

#ifndef _HW4_HEXBOARD_H_
#define _HW4_HEXBOARD_H_

#include <string>
#include <inttypes.h>

#include "./Graph.h"
#include "./ShortestPath.h"

namespace hw4 {

class HexBoard {
 public:
  // This enum type enumerates the different possible occupancy states
  // of a cell on a hex board.
  typedef enum {
    EMPTY = 0, RED = 1, BLUE = 2
  } occupancy;

  // Constructs and empty hex board of the given dimensions. Note that
  // width and height must be non-negative, otherwise the code will
  // exit() with EXIT_FAILURE.
  HexBoard(uint16_t width, uint16_t height);
  virtual ~HexBoard() { }

  // Returns the current occupancy of a board position. If x/y are
  // beyond the end of the board, will exit() with EXIT_FAILURE.
  occupancy GetOccupancy(uint16_t x, uint16_t y) const;

  // Attempts to change the occupancy of a board position. If x/y
  // are beyond the end of the board, or if color is EMPTY, will
  // exit() with EXIT_FAILURE. Returns true if the position occupancy
  // was set (i.e., the position was previously EMPTY), otherwise
  // returns false.
  bool SetOccupancy(uint16_t x, uint16_t y, occupancy color);

  // Returns a const reference to the internal graph representation
  // of the hex board.
  const Graph<occupancy, double>& GetGraph() const;

  // Returns whether blue has a winning position.
  bool HasBlueWon() const;

  // Returns whether red has a winning position.
  bool HasRedWon() const;

  // Returns a string representation of the current hex
  // board position.
  std::string ToString() const;

  // Returns the dimensions of the board.
  uint16_t get_width() const { return width_; }
  uint16_t get_height() const { return height_; }

 private:
  uint16_t width_, height_;
  Graph<occupancy, double> graph_;
};

} // end namespace hw4

#endif // _HW4_HEXBOARD_H_