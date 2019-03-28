#include "exhaustive.h"

#include <iostream>
#include <random>
#include <set>
#include <cassert>
#include <functional>

using namespace std;


namespace algorithm {

  
  Exhaustive::Exhaustive(Situation* s) : Algorithm(s) {
    //calculate();
    states.resize(1000);
    state::State state = s->state;
    for(int i = 0; i < int(states.size()); i++) {
      states[i] = state;
      auto nStates = nextStates(state);
      state = nStates[rand()%nStates.size()];
    }
    s->state = states[0];
  }

  void Exhaustive::calculate() {
    states = nextStates(situation->state);
    situation->state = states[0];
  }

  void Exhaustive::makeMove() {
    if(curState + 1 < int(states.size()))
      situation->state = states[++curState];
  }

  // All possible states that can come after state
  vector<state::State> Exhaustive::nextStates(state::State state) {
    mission::Mission &mission = situation->mission;
    vector<state::State> states;

    function<void(state::State, int)> recursive = [&](state::State s, int rId){
      if (rId == int(state.robots.size())) {
        states.push_back(s);
        return;
      }
      state::robot r = s.robots[rId];

      // do nothing
      recursive(s, rId + 1);

      if (r.item == -1) { // pick up item
        for (int i : mission.adjItems(r.pos)) {
          state::State ns = s;
          ns.robots[rId].item = i;

          recursive(ns, rId + 1);
        }
      }

      if (r.item != -1) { // leave item
        for(int sId : mission.adjStations(r.pos)) {
          state::station station = s.stations[sId];
          if (station.order == -1)
            continue;
          mission::order order = mission.orders[station.order]; 
          for (int i = 0; i < int(order.items.size()); i++)
            if (!station.fulfilled[i] && order.items[i] == r.item) {
              situation->state = s;
              deliverItem(r.item, sId);
              state::State ns = situation->state;
              ns.robots[rId].item = -1;
              
              recursive(ns, rId + 1);
            }
        }
      }

      // move
      for (int pos : mission.adjPos(r.pos)) {
        situation->state = s;
        if (!occupied(pos)) {
          state::State ns = s;
          ns.robots[rId].pos = pos;

          recursive(ns, rId + 1);
        }
      }
    };

    recursive(state, 0);
    return states;
  }
}