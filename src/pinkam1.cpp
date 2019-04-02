#include "pinkam1.h"

#include <iostream>
#include <random>
#include <set>
#include <cassert>

using namespace std;


namespace algorithm {

  Pinkam1::Pinkam1(Situation* s) : Algorithm(s) {
    state::State &state = situation->state;
    mission::Mission &mission = situation->mission;
    assignedItem.assign(state.robots.size(), -1);
    for(int i = 0; i < int(state.stations.size()); i++) {
        state.stations[i].order = i < int(mission.orders.size()) ? i : -1;

    }
  }


  void Pinkam1::makeMove() {
    mission::Mission &mission = situation->mission;
    state::State &state = situation->state;

    for(int id = 0; id < int(state.robots.size()); id++) {
      state::robot& r = state.robots[id];
      //first case, we are carrying an item!
      if(r.item != -1) {
        bool foundStation = false;
        for(int s : mission.adjStations(r.pos)) {
          if(s == id) { //This is my station! Pinkam1 only delivers to the station with the same id
            foundStation = true;
            deliverItem(r.item, s);
            
            r.item = -1;
            assignedItem[id] = -1;
            break;
          }
        }
        if(r.item == -1) {
          // Pick a new item to deliver to my station
          assignItem(r, id);
        }
        // Only move if we didn't deliver anything
        else {
          moveTowards(r, mission.stations[id].coors);
        }
      }
      // Second case, we are not carrying an item. 
      else {
        if(assignedItem[id] == -1) {
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
          if(i == assignedItem[id]) {
            foundShelf = true;
            r.item = assignedItem[id];
          }
        }
        // If we didn't pick something up, we can move instead
        if(!foundShelf) {
          moveTowards(r, mission.items[assignedItem[id]].shelfCoors);
        }

      }
    }

  }

  // Assign a new item to this robot. Returns false if no new item could be assigned.
  // Since this is non collaborative Pinkam, it can only pick an item from its paired station
  bool Pinkam1::assignItem(state::robot &r, int id) {
    mission::Mission &mission = situation->mission;
    state::State &state = situation->state;
    state::station s = state.stations[id];

    // If this station has no order assigned, this robot is out of work
    if(s.order == -1) {
      assignedItem[id] = -1;
      return false;
    }

    int bestDistance = 1e9;
    int chosenItem = -1;
    // Of all the available items at this station, calculate which one is closest to reach
    for(int i = 0; i < int(mission.orders[s.order].items.size()); i++) {
      if(!s.fulfilled[i]) {
        int itemId = mission.orders[s.order].items[i];
        // Their might be up to 4 adjacent squares to the shelf
        vector<int> adj = mission.adjPos(mission.items[itemId].shelfCoors);
        for(int pos : adj) {
          int dist = distance(r.pos, pos);
          if(dist < bestDistance) {
            bestDistance = dist;
            chosenItem = itemId;
          }
        }        
      }
    }
    if(chosenItem != -1) {
      assignedItem[id] = chosenItem;
      return true;
    }
    // All remaining items for this station was already assigned
    else {
      assert(false); // This should never happen in pinkam1
      assignedItem[id] = -1;
      return false; 

    }
  }
}