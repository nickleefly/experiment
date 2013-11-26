// Copyright 2013 Steve Gribble -- gribble (at) gmail (dot) com

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>
#include <inttypes.h>

#include "./HexBoard.h"

// Prints the game instructions, then asks the user whether red or
// blue should move first. Returns either "red" or "blue", depending
// on what the user inputs.
std::string PrintIntro(const hw4::HexBoard &board);

// Gets the next move from the user. Returns the move coordinates
// through the variables "x" and "y".  Returns true if it was able to
// get a uint16_t move, false otherwise.
bool GetNextMove(uint16_t *x, uint16_t *y);

int main(int argc, char **argv) {
  // Get the board dimensions from argv.
  uint64_t width, height;
  if (argc != 3) {
    std::cerr << "Usage: ./gameplay boardwidth boardheight" << std::endl;
    return EXIT_FAILURE;
  }
  width = std::stoull(argv[1]);
  height = std::stoull(argv[2]);
  if ((width > UINT16_MAX) || (height > UINT16_MAX)) {
    std::cerr << "width/height must be < 65536." << std::endl;
    return EXIT_FAILURE;
  }

  // Prep the hexboard, print out the game intro.
  hw4::HexBoard hb(width, height);
  std::string first = PrintIntro(hb);
  bool blue_moves_next = (first == "blue");

  // Enter the main game loop.
  while (true) {
    std::cout << "Enter " << (blue_moves_next ? "blue" : "red")
              << "'s next move (e.g., \"4 7\"): "
              << std::endl;

    uint16_t x, y;
    if (!GetNextMove(&x, &y)) {
      std::cout << "Sorry, please input two integers for x and y." << std::endl;
      continue;
    }

    if (!hb.SetOccupancy(x, y,
          blue_moves_next ? hw4::HexBoard::BLUE : hw4::HexBoard::RED)) {
      std::cout << "Sorry, that cell is occupied already." << std::endl;
      continue;
    }
    std::cout << std::endl << hb.ToString() << std::endl;

    if (hb.HasBlueWon()) {
      std::cout << "Blue has won!" << std::endl;
      exit(EXIT_SUCCESS);
    }
    if (hb.HasRedWon()) {
      std::cout << "Red has won!" << std::endl;
      exit(EXIT_SUCCESS);
    }

    blue_moves_next = !blue_moves_next;
  }
  return EXIT_SUCCESS;
}

std::string PrintIntro(const hw4::HexBoard &board) {
  // Print out the game header.
  std::cout << std::endl;
  std::cout << "Welcome to hexboard! Written by gribble [at] gmail.com."
            << std::endl << std::endl;
  std::cout << board.ToString() << std::endl;
  std::cout << "Blue (B) connects West<-->East, "
            << "Red (R) connects North<-->South."
            << std::endl;
  std::cout << "The top-left of the board is x=0, y=0 and the bottom-right"
            << std::endl << "of the board is x=" << board.get_width() - 1
            << ", y=" << board.get_height() - 1 << "." << std::endl;

  // Ask who should move first.
  std::cout << std::endl;
  std::string movefirst;
  while ((movefirst != "r") && (movefirst != "b") &&
         (movefirst != "red") && (movefirst != "blue")) {
    std::cout << "Who would you like to move first: Blue (B) or Red (R)?"
              << std::endl;
    std::cin >> movefirst;
    if (!std::cin.good()) {
      std::cerr << "Reading from standard input failed." << std::endl;
      exit(EXIT_FAILURE);
    }
    std::transform(movefirst.begin(), movefirst.end(), movefirst.begin(),
                   ::tolower);
  }

  if ((movefirst == "r") || (movefirst == "red")) {
    movefirst = "red";
    return movefirst;
  }
  return "blue";
}

bool GetNextMove(uint16_t *x, uint16_t *y) {
  uint16_t inx, iny;
  std::cin >> inx;
  std::cin >> iny;
  if (std::cin.good()) {
    *x = inx;
    *y = iny;
    return true;
  }
  if (std::cin.eof()) {
    exit(EXIT_FAILURE);
  }
  std::cin.clear();
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  return false;
}