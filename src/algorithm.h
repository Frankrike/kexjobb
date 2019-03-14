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
public:
  Algorithm(Situation* s);  
  void makeMove();
};

}
#endif