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
    assignedTo.resize(state.stations.size());
    for(int i = 0; i < int(state.stations.size()); i++) {
        state.stations[i].order = i < int(mission.orders.size()) ? i : -1;
        if (state.stations[i].order != -1) {
          assignedTo[i].push_back(-1);
        }
    }
  }


  void Pinkam1::makeMove() {
    mission::Mission &mission = situation->mission;
    state::State &state = situation->state;

    for(int id = 0; id < int(state.robots.size()); id++) {
      cout << id << " is assigned " << assignedItem[id] << endl;
      state::robot& r = state.robots[id];
      //first case, we are carrying an item!
      if(r.item != -1) {
        bool foundStation = false;
        for(int s : mission.adjStations(r.pos)) {
          if(s == id) { //This is my station! Pinkam1 only delivers to the station with the same id
            cout << r.item << " " << s << endl;
            foundStation = true;
            cout << "Delivering for id " << id << " and station " << s << endl;
            deliverItem(r.item, s);
            assignedTo[s][r.item] = -1;
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
            cout << id << " was assigned " << i << endl;
          }
        }
        if(foundShelf) {
          // Do nothing, we spent this round picking up the item
        }
        else {
          moveTowards(r, mission.items[assignedItem[id]].shelfCoors);
        }

      }
    }

  }

  // Assign a new item to this robot. Returns false if no new item could be assigned.
  // Since this is Pinkam1, it can only pick an item from it's paired station
  // 
  bool Pinkam1::assignItem(state::robot &r, int id) {
    cout << "Trying to assign" << endl;
    mission::Mission &mission = situation->mission;
    state::State &state = situation->state;
    state::station s = state.stations[id];
    // If this station has no order assigned, this robot is out of work
    if(s.order == -1) {
      cout << "Robot paired with station " << id << "no order!" << endl;
      assignedItem[id] = -1;
      return false;
    }
    cout << "Robot paired with station " << id << "has order " << s.order << endl;

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
    else {
      cout << "failed assigning" << endl;
      assignedItem[id] = -1;
      return false; // All remaining items for this station was already assigned (should't really happpen in pinkam1)

    }
  }

  

}