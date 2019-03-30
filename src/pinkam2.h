#ifndef PINKAM2_H_
#define PINKAM2_H_

#include "algorithm.h"

namespace algorithm {
  struct assignment {
    int item;
    int itemPosInOrder;
    int station; 
  };


// Implementation of the second, collaborative algorithm proposed by Pinkam et al.

class Pinkam2 : public Algorithm {

public:
  void makeMove();
  Pinkam2(Situation* s);
private:
  bool assignItem(state::robot &r, int id); // Assigns a new item to robot with the given id
  void deliverItem(int item, int station);
  vector<assignment> assignedItem;
  vector<vector<int> > assignedTo;
};

}
#endif