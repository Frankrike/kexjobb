#include "algorithm.h"

#include <iostream>
#include <random>
#include <set>
#include <cassert>

using namespace std;


namespace algorithm {

  Algorithm::Algorithm(Situation* s) : situation(s) {}

  void Algorithm::makeMove() {
    mission::Mission &mission = situation->mission;
    state::State &state = situation->state;

    for(state::robot &r : state.robots) {
      bool done = false;

      // pick up item
      if (mission.adjItems(r.pos).size() != 0 && r.item == -1) {
        set<int> orderedItems;
        for(state::station station : state.stations)
          if (station.order != -1) {
            mission::order order = mission.orders[station.order];
            for (int i = 0; i < int(order.items.size()); i++)
              if (!station.fulfilled[i])
                orderedItems.insert(order.items[i]);
          }

        for (int i : mission.adjItems(r.pos))
          if (orderedItems.count(i)) {
            r.item = i;
            done = true;
            break;
          }
      }
      if (done)
        continue;

      // leave item
      if (r.item != -1)
        for(int s : mission.adjStations(r.pos)) {
          state::station station = state.stations[s];
          if (station.order == -1)
            continue;
          mission::order order = mission.orders[station.order]; 
          for (int i = 0; i < int(order.items.size()); i++)
            if (!station.fulfilled[i] && order.items[i] == r.item) {
              deliverItem(r.item, s);
              r.item = -1;
              done = true;
              break;
            }
          if (done)
            break;
        }
      if (done)
        continue;

      // move randomly
      vector<int> adj = mission.adjPos(r.pos);
      if (adj.size() != 0) {
        int pos = adj[rand()%adj.size()];
        if (!occupied(pos))
          r.pos = pos;
      }
    }
  }

  void Algorithm::deliverItem(int item, int s){
    mission::Mission &mission = situation->mission;
    state::State &state = situation->state;

    state::station &station = state.stations[s];
    assert(station.order != -1);
    mission::order order = mission.orders[station.order];

    for(int i = 0; i < int(order.items.size()); i++)
      if (!station.fulfilled[i] && order.items[i] == item) {
        station.fulfilled[i] = true;
        item = -1;
        break;
      }
    assert(item == -1);

    bool stationFull = true;
    for (bool b : station.fulfilled)
      stationFull &= b;

    if (stationFull) { // updateOrder
      int highestOrder = -1;
      for(state::station station : state.stations)
        highestOrder = max(highestOrder, station.order);

      int nextOrder = highestOrder + 1;
      if (nextOrder == int(mission.orders.size()))
        nextOrder = -1;

      station.order = nextOrder;
      if (station.order != -1) {
        station.fulfilled.assign(
            mission.orders[station.order].items.size(), false);
      }
    }
  }

  bool Algorithm::occupied(int pos) {
    for(state::robot r : situation->state.robots)
      if (r.pos == pos)
        return true;
    return false;
  }
}
