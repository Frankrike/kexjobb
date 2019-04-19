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
    empty.itemPosInOrder = -1;
    empty.station = -1;
    assignedItem.assign(state.robots.size(), empty);
    assignedTo.resize(state.stations.size());
    for(int i = 0; i < int(state.stations.size()); i++) {
        state.stations[i].order = i < int(mission.orders.size()) ? i : -1;
        if (state.stations[i].order != -1) {
          assignedTo[i].assign(mission.orders[state.stations[i].order].items.size(), -1);
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
            assignedTo[s][assignedItem[id].itemPosInOrder] = -1; // Must be before deliverItem, otherwise this might update a new order
            deliverItem(assignedItem[id].itemPosInOrder, s);

            r.item = -1;
            assignedItem[id].item = -1;
            assignedItem[id].station = -1;
            assignedItem[id].itemPosInOrder = -1;
            break;
          }
        }
        if(foundStation) {
          // Pick a new item to deliver to my station
          assignItem(r, id);
        }
        // Only move if we didn't deliver anything
        else {
          moveTowards2(r, mission.stations[assignedItem[id].station].coors);
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
          moveTowards2(r, mission.items[assignedItem[id].item].shelfCoors);
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
      if(s.order == -1) continue;
      // Of all the available items at this station, calculate which one is closest to reach
      for(int i = 0; i < int(mission.orders[s.order].items.size()); i++) {
        if(!s.fulfilled[i] && assignedTo[stationId][i] == -1) {
          int itemId = mission.orders[s.order].items[i];
          // Their might be up to 4 adjacent squares to the shelf
          vector<int> adj = mission.adjPos(mission.items[itemId].shelfCoors);
          for(int pos : adj) {
            int dist = collisionDistance(r.pos, pos);
            if(dist < bestDistance && dist >= 0) {
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
    // No stations has unassigned items that are currently reachable for robot r
    else {
      assignedItem[id].item = -1;
      assignedItem[id].station = -1;
      assignedItem[id].itemPosInOrder = -1;
      return false; 

    }
  }

  void Pinkam2::deliverItem(int itemPosInOrder, int s){
    mission::Mission &mission = situation->mission;
    state::State &state = situation->state;

    state::station &station = state.stations[s];
    assert(station.order != -1);
    mission::order order = mission.orders[station.order];

    assert(station.fulfilled[itemPosInOrder] == false);

    //Deliver item
    station.fulfilled[itemPosInOrder] = true;

    //Check if the station needs a new order
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

  string Pinkam2::name() {
    return "pinkam2";
  }
}