// Copyright 2013 Steve Gribble -- gribble (at) gmail (dot) com

#ifndef _HW3_UNIONFIND_FOREST_H_
#define _HW3_UNIONFIND_FOREST_H_

#include <vector>
#include <inttypes.h>

namespace hw3 {

// A UnionFindForest is a disjoint set forest data structure (see
// http://en.wikipedia.org/wiki/Disjoint-set_data_structure). It is
// used to keep track of sets of elements, partitioned into disjoint
// subsets, and it efficiently supports Union and Find operations.
class UnionFindForest {
 public:
  // Creates a new union find forest with 'num_elements' disjoint
  // items, ranging from item 0 to item (num_elements-1).
  UnionFindForest(uint32_t num_elements);
  virtual ~UnionFindForest() { }

  // If 'x' and 'y' are in disjoint sets, then this method will
  // join those two disjoint sets together.
  void Union(uint32_t x, uint32_t y);

  // Finds the representative element of the set of which 'x' is a
  // member.
  uint32_t Find(uint32_t x);

 private:
  typedef struct {
    uint32_t parent;
    uint32_t rank;
  } UFElement;

  std::vector<UFElement> elements_;
};

} // end namespace hw3

#endif // _HW3_UNIONFIND_FOREST_H_