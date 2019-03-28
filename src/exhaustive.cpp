#include "exhaustive.h"

#include <iostream>
#include <random>
#include <set>
#include <cassert>
#include <functional>
#include <vector>
#include <queue>

using namespace std;


namespace algorithm {

  
  Exhaustive::Exhaustive(Situation* s) : Algorithm(s) {
    calculate();
  }

  void Exhaustive::calculate() {
    state::State state = situation->state;
    mission::Mission mission = situation->mission;
    int n = state.maxEncoding(mission);
    cout << "Starting total search. Max " << n << " states." << endl;

    vector<int> par(n, -1);
    queue<int> q;

    auto add = [&](int u, int p) {
      if (par[u] != -1)
        return;
      par[u] = p;
      q.push(u);
    };
    int start = state.encode(mission), goal = -1, steps = 0;
    add(start, -2);

    while(!q.empty()) {
      int u = q.front(); q.pop();
      state::State curState = state::State(u, mission);

      situation->state = curState;
      if (situation->missionCompleted()) {
        goal = u;
        break;
      }

      steps ++;
      if (steps%1000 == 0) {
        cout << "At: " << u << ", step " << steps << endl;
        cout << nextStates(curState).size() << " adjacent. " << endl;
        situation->state = curState;
        cout <<  situation->debugView() << endl;
      }

      for (int v : nextStates(curState))
        add(v, u);
    }
    cout << "Finished. Start: " << start << ", goal: " << goal << endl;

    if (goal == -1) {
      cout << "Failed to find solution." << endl;
      return;
    }

    int cur = goal;
    while(cur != -2) {
      cout << "Backtracking at " << cur << endl;
      states.insert(states.begin(), state::State (cur, mission));
      cur = par[cur];
    }
    situation->state = states[0];
  }

  void Exhaustive::makeMove() {
    if(++curState < int(states.size()))
      situation->state = states[curState];
  }

  // All possible states that can come after state, encoded
  vector<int> Exhaustive::nextStates(state::State state) {
    mission::Mission &mission = situation->mission;
    vector<int> states;

    function<void(state::State, int)> recursive = [&](state::State s, int rId){
      if (rId == int(state.robots.size())) {
        states.push_back(s.encode(mission));
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