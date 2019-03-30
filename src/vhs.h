#ifndef VHS_H_
#define VHS_H_

#include "algorithm.h"
#include "pinkam2.h"

namespace algorithm {

struct pairing {
  int dist;
  int robotId;
  int item;
  int itemPosInOrder;
  int station; 

  bool operator<(const pairing& rhs) const {  return dist > rhs.dist; }
};


// Implementation of our own heuristic. 

class Vhs : public Algorithm {
public:
  void makeMove();
  Vhs(Situation* s);
private:
  //bool assignItem(state::robot &r, int id); // Assigns a new item to robot with the given id
  void assignAll();
  void deliverItem(int item, int station);
  vector<assignment> assignedItem;
  vector<vector<int> > assignedTo;
};

}
#endif