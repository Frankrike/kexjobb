#ifndef PINKAM1_H_
#define PINKAM1_H_

#include "algorithm.h"

namespace algorithm {



// Implementation of the first, non collaborative algorithm proposed by Pinkam et al.
// Requires that the number of robots and stations are equal, as they will be paired
// The first robot in the input, is paired with the first station in the input, an so on

class Pinkam1 : public Algorithm {
public:
  void makeMove();
  Pinkam1(Situation* s);
private:
  bool assignItem(int id); // Assigns a new item to robot with the given id
  vector<int> assignedItem;
  vector<vector<int> > assignedTo;
};

}
#endif