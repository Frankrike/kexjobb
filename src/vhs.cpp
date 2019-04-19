#include "vhs.h"

#include <iostream>
#include "random.h"
#include <set>
#include <cassert>
#include <queue>

using namespace std;


namespace algorithm {

  Vhs::Vhs(Situation* s) : Algorithm(s) {
    state::State &state = situation->state;
    mission::Mission &mission = situation->mission;

    assignment empty;
    empty.item = -1;
    empty.station = -1;
    empty.itemPosInOrder = -1;
    assignedItem.assign(state.robots.size(), empty);
    assignedTo.resize(state.stations.size());
    for(int i = 0; i < int(state.stations.size()); i++) {
        state.stations[i].order = i < int(mission.orders.size()) ? i : -1;
        if (state.stations[i].order != -1) {
          assignedTo[i].assign(mission.orders[state.stations[i].order].items.size(), -1);
        }
    }
  }

  void Vhs::makeMove() {
    mission::Mission &mission = situation->mission;
    state::State &state = situation->state;

    // For each robot that is not carrying anything, reassign
    assignAll();

    // For each robot, move/deliver/pickup
    for(int id = 0; id < int(state.robots.size()); id++) {
      state::robot& r = state.robots[id];
      //first case, we are carrying an item!
      if(r.item != -1) {
        bool foundStation = false;
        for(int s : mission.adjStations(r.pos)) {
          if(s == assignedItem[id].station) {
            foundStation = true;
            assignedTo[s][assignedItem[id].itemPosInOrder] = -1; // Must be before delivery
            deliverItem(assignedItem[id].itemPosInOrder, s);            
            r.item = -1;
            assignedItem[id].item = -1;
            assignedItem[id].station = -1;
            assignedItem[id].itemPosInOrder = -1;
            break;
          }
        }
        // Only move if we didn't deliver anything
        if(!foundStation) {
          moveTowards2(r, mission.stations[assignedItem[id].station].coors);
        }
      }
      // Second case, we are not carrying an item. 
      else {
        if(assignedItem[id].item == -1) {
          // Assignment must have failed
          moveAround(r);
          continue;
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

  void Vhs::deliverItem(int itemPosInOrder, int s){
    mission::Mission &mission = situation->mission;
    state::State &state = situation->state;

    state::station &station = state.stations[s];
    assert(station.order != -1);
    mission::order order = mission.orders[station.order];

    assert(station.fulfilled[itemPosInOrder] == false);
    station.fulfilled[itemPosInOrder] = true;

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

  void Vhs::assignAll() {
    mission::Mission &mission = situation->mission;
    state::State &state = situation->state;

    //Invalidate all previous assignments
   for(int i = 0; i < assignedTo.size(); i++) {
     for(int j = 0; j < assignedTo[i].size(); j++) {
       // Leave it if a robot already carries this item
       if(assignedTo[i][j] != -1) {
        if(state.robots[assignedTo[i][j]].item == -1) assignedTo[i][j] = -1;
       }
     }
   }
   assignment empty;
   empty.item = -1;
   empty.station = -1;
   empty.itemPosInOrder = -1;
   for(int i = 0; i < assignedItem.size(); i++) {
     // Leave it if the robot already carries the item
     if(state.robots[i].item == -1) assignedItem[i] = empty;
   }

    priority_queue<pairing> q;
    for(int robotId = 0; robotId < int(state.robots.size()); robotId++) {
      state::robot &r = state.robots[robotId];
      // This robot has already picked up an item, do not attempt reassignment
      if(r.item != -1) {
        continue;
      }
      for(int stationId = 0; stationId < int(state.stations.size()); stationId++) {
        state::station s = state.stations[stationId];
        if(s.order == -1) continue;
        // Of all the available items at this station, calculate which one is closest to reach
        for(int i = 0; i < int(mission.orders[s.order].items.size()); i++) {
          if(s.fulfilled[i])  continue;
          
          // Inv: We now have a robot that is not carrying an item, and a station that needs an item

          int itemId = mission.orders[s.order].items[i];

          vector<int> adj = mission.adjPos(mission.items[itemId].shelfCoors);
          int bestDistance = 1e9;
          for(int pos : adj) {
            int dist = collisionDistance(r.pos, pos);
            if(dist == -1) continue;
            if(dist < bestDistance) bestDistance = dist;
          }
          pairing p;
          p.dist = bestDistance;
          p.robotId = robotId;
          p.item = itemId;
          p.itemPosInOrder = i;
          p.station = stationId;
          q.push(p);
        }
      }
    }

    while(!q.empty()) {
      pairing p = q.top();
      q.pop();
      // Someone already got this item order
      if(assignedTo[p.station][p.itemPosInOrder] != -1) continue;
      // This robot already has an assignment
      if(assignedItem[p.robotId].item != -1) continue;

      //Make this assignment
      assignedTo[p.station][p.itemPosInOrder] = p.robotId;
      assignment a;
      a.item = p.item;
      a.itemPosInOrder = p.itemPosInOrder;
      a.station = p.station;
      assignedItem[p.robotId] = a;  
    }
  }

  string Vhs::name() {
    return "vhs";
  }
}
  



