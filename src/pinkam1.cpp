#include "pinkam1.h"

#include <iostream>
#include <random>
#include <set>
#include <cassert>

using namespace std;


namespace algorithm {

  void Pinkam1::makeMove() {
    mission::Mission &mission = situation->mission;
    state::State &state = situation->state;

    for(state::robot &r : state.robots) {
      cout << "new robot" << endl;
      //first case, we are carrying an item!
      if(r.item != -1) {
        bool foundStation = false;
        for(int s : mission.adjStations(r.pos)) {
          if(s == r.fixedStation) { //This is my station! Pinkam1 only delivers to the station with the same id
            cout << r.item << " " << s << endl;
            foundStation = true;
            deliverItem(r.item, s);
            r.item = -1;
            continue;
          }
        }
        if(foundStation) {
          // Pick a new item to deliver to my station
          assignItem(r);
        }
        else {
          moveTowards(r, mission.stations[r.fixedStation].coors);
        }
      }
      // Second case, we are not carrying an item. 
      else {
        if(r.assignedItem == -1) {
          // Try to assign a new item, if this doesn't work, just try to stay out of the way
          if(!assignItem(r)) {
            moveAround(r);
            continue;
          }
        }
        //inv: r has an assigned item
        bool foundShelf = false;
        // See if any of the shelves we're near has this item
        for(int i : mission.adjItems(r.pos)) {
          if(i == r.assignedItem) {
            foundShelf = true;
            r.item = r.assignedItem;
            cout << r.fixedStation << " was assigned " << i << endl;
          }
        }
        if(foundShelf) {
          moveTowards(r, mission.stations[r.fixedStation].coors);
        }
        else {
          moveTowards(r, mission.items[r.assignedItem].shelfCoors);
        }

      }
    }

  }

  // Assign a new item to this robot. Returns false if no new item could be assigned.
  bool Pinkam1::assignItem(state::robot &r) {
    mission::Mission &mission = situation->mission;
    state::State &state = situation->state;
    state::station s = state.stations[r.fixedStation];
    // If this station has no order assigned, this robot is out of work
    if(s.order == -1) {
      cout << "Robot paired with station " << r.fixedStation << "no order!" << endl;
      r.assignedItem = -1;
      return false;
    }
    cout << "Robot paired with station " << r.fixedStation << "has order " << s.order << endl;
    for(int i = 0; i < int(mission.orders[s.order].items.size()); i++) {
      // If this item is still needed, and no other robot is fetching it
      if(!s.fulfilled[i] && s.assignedTo[i] == -1) {
        r.assignedItem = mission.orders[s.order].items[i];
        cout << "Robot paired with station " << r.fixedStation << " was assigned item " << r.assignedItem << endl;
        return true;
      }
    }
    r.assignedItem = -1;
    return false; // All remaining items for this station was already assigned (should't really happpen in pinkam1)
  }

  

}