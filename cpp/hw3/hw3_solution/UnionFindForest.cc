// Copyright 2013 Steve Gribble -- gribble (at) gmail (dot) com

#include "./UnionFindForest.h"

namespace hw3 {

// Initializes the forest with disjoint elements.
UnionFindForest::UnionFindForest(uint32_t num_elements) {
  elements_.reserve(num_elements);
  for (uint32_t i = 0U; i < num_elements; ++i) {
    UFElement next_el = {i, 0};
    elements_.push_back(next_el);
  }
}

// Implements "union by rank."
void UnionFindForest::Union(uint32_t x, uint32_t y) {
  uint32_t xroot = Find(x), yroot = Find(y);
  if (xroot == yroot)
    return;

  if (elements_[xroot].rank < elements_[yroot].rank) {
    elements_[xroot].parent = yroot;
  } else if (elements_[xroot].rank > elements_[yroot].rank) {
    elements_[yroot].parent = xroot;
  } else {
    elements_[yroot].parent = xroot;
    elements_[xroot].rank = elements_[xroot].rank + 1;
  }
}

// Implements "path compression."
uint32_t UnionFindForest::Find(uint32_t x) {
  if (elements_[x].parent != x) {
    elements_[x].parent = Find(elements_[x].parent);
  }
  return elements_[x].parent;
}

} // end namespace hw3