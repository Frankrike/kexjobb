#include "algorithm.h"

#include <iostream>

using namespace std;


namespace algorithm {

  Algorithm::Algorithm(Situation* s) : situation(s) {}

  void Algorithm::makeMove() {
    mission::Mission &mission = situation->mission;
    state::State &state = situation->state;
    for(state::robot &r : state.robots) {
        pair<int, int> coors = mission.toCoors(r.pos);
        int newPos;

        newPos = mission.toPos(make_pair(coors.first+1, coors.second));
        if(newPos != -1) {
            r.pos = newPos;
            continue;
        }

        newPos = mission.toPos(make_pair(coors.first-1, coors.second));
        if(newPos != -1) {
            r.pos = newPos;
            continue;
        }

        newPos = mission.toPos(make_pair(coors.first, coors.second+1));
        if(newPos != -1) {
            r.pos = newPos;
            continue;
        }

        newPos = mission.toPos(make_pair(coors.first, coors.second-1));
        if(newPos != -1) {
            r.pos = newPos;
            continue;
        }
    }
  }

}
