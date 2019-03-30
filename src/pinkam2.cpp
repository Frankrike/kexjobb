#include "pinkam2.h"

#include <iostream>
#include <random>
#include <set>
#include <cassert>

using namespace std;


namespace algorithm {
  

  Pinkam2::Pinkam2(Situation* s) : Algorithm(s) {
    state::State &state = situation->state;
    mission::Mission &mission = situation->mission;

    assignment empty;
    empty.item = -1;
    empty.station = -1;
    assignedItem.assign(state.robots.size(), empty);
    assignedTo.resize(state.stations.size());
    for(int i = 0; i < int(state.stations.size()); i++) {
        state.stations[i].order = i < int(mission.orders.size()) ? i : -1;
        if (state.stations[i].order != -1) {
          for(int j = 0; j < int(mission.orders[state.stations[i].order].items.size()); j++) {
            assignedTo[i].push_back(-1);
          }
        }
    }
  }

  void Pinkam2::makeMove() {
    mission::Mission &mission = situation->mission;
    state::State &state = situation->state;

    for(int id = 0; id < int(state.robots.size()); id++) {
      state::robot& r = state.robots[id];
      //first case, we are carrying an item!
      if(r.item != -1) {
        bool foundStation = false;
        for(int s : mission.adjStations(r.pos)) {
          if(s == assignedItem[id].station) {
            foundStation = true;
            deliverItem(r.item, s);
            assignedTo[s][assignedItem[id].itemPosInOrder] = -1;
            r.item = -1;
            assignedItem[id].item = -1;
            assignedItem[id].station = -1;
            break;
          }
        }
        if(r.item == -1) {
          // Pick a new item to deliver to my station
          assignItem(r, id);
        }
        // Only move if we didn't deliver anything
        else {
          moveTowards(r, mission.stations[assignedItem[id].station].coors);
        }
      }
      // Second case, we are not carrying an item. 
      else {
        if(assignedItem[id].item == -1) {
          // Try to assign a new item, if this doesn't work, just try to stay out of the way
          if(!assignItem(r, id)) {
            moveAround(r);
            continue;
          }
        }
        //inv: r has an assigned item
        bool foundShelf = false;
        // See if any of the shelves we're near has this item
        for(int i : mission.adjItems(r.pos)) {
          if(i == assignedItem[id].item) {
            foundShelf = true;
            r.item = assignedItem[id].item;
          }
        }
        // If we didn't pick something up, we can move instead
        if(!foundShelf) {
          moveTowards(r, mission.items[assignedItem[id].item].shelfCoors);
        }

      }
    }

  }

  // Assign a new item to this robot. Returns false if no new item could be assigned.
  bool Pinkam2::assignItem(state::robot &r, int id) {
    mission::Mission &mission = situation->mission;
    state::State &state = situation->state;

    int bestDistance = 1e9;
    int chosenItem = -1;
    int chosenStation = -1;
    int chosenItemPosInOrder = -1;
    for(int stationId = 0; stationId < int(state.stations.size()); stationId++) {
      state::station s = state.stations[stationId];
      cout << "Station: " << stationId << " has order " << s.order << endl;
      if(s.order == -1) continue;
      // Of all the available items at this station, calculate which one is closest to reach
      for(int i = 0; i < int(mission.orders[s.order].items.size()); i++) {
        cout << assignedTo[stationId].size() << endl;
        cout << i << " " << s.fulfilled[i] << " " << assignedTo[stationId][i] << endl;
        if(!s.fulfilled[i] && assignedTo[stationId][i] == -1) {
          cout << "passed if statement" << endl;
          int itemId = mission.orders[s.order].items[i];
          // Their might be up to 4 adjacent squares to the shelf
          vector<int> adj = mission.adjPos(mission.items[itemId].shelfCoors);
          for(int pos : adj) {
            int dist = distance(r.pos, pos);
            cout << "dist is " << dist << endl;
            if(dist < bestDistance) {
              bestDistance = dist;
              chosenItem = itemId;
              chosenItemPosInOrder = i;
              chosenStation = stationId;
            }
          }        
        }
      }
    }
    if(chosenItem != -1) {
      assignedItem[id].item = chosenItem;
      assignedItem[id].itemPosInOrder = chosenItemPosInOrder;
      assignedItem[id].station = chosenStation;
      assignedTo[chosenStation][chosenItemPosInOrder] = id;
      return true;
    }
    // All remaining items for this station was already assigned
    else {
      assignedItem[id].item = -1;
      assignedItem[id].station = -1;
      assignedItem[id].itemPosInOrder = -1;
      return false; 

    }
  }

  void Pinkam2::deliverItem(int item, int s){
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
      for(state::station station : state.stations) {
        int orderHere = station.order;
        if (orderHere == -1)
          orderHere = mission.orders.size();
        highestOrder = max(highestOrder, orderHere);
      }

      int nextOrder = highestOrder + 1;
      if (nextOrder >= int(mission.orders.size()))
        nextOrder = -1;
      station.order = nextOrder;
      if (station.order != -1) {
        station.fulfilled.assign(
            mission.orders[station.order].items.size(), false);
        assignedTo[s].assign(mission.orders[station.order].items.size(), -1);
      }
    }
  }

}