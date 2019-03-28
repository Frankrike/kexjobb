#ifndef ALGORITHM_H_
#define ALGORITHM_H_

#include "situation.h"

namespace algorithm {

class Algorithm {
protected:
  Situation* situation;
  void deliverItem(int item, int station);
  void moveTowards(state::robot &r, pair<int, int> coors);
  void moveAround(state::robot &r);
  bool occupied(int pos);
  int BFS(int start, int destination);
  int distance(int start, int destination);
public:
  Algorithm(Situation* s);  
  virtual void makeMove();
};

}
#endif