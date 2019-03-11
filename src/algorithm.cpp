#include "algorithm.h"

#include <iostream>
#include <random>

using namespace std;


namespace algorithm {

  Algorithm::Algorithm(Situation* s) : situation(s) {}

  void Algorithm::makeMove() {
    mission::Mission &mission = situation->mission;
    state::State &state = situation->state;
    for(state::robot &r : state.robots) {
      vector<int> adj = mission.adjPos(r.pos);
      if (adj.size() != 0)
        r.pos = adj[rand()%adj.size()];
    }
  }
}
