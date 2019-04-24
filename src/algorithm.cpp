#include "algorithm.h"

#include <iostream>
#include "random.h"
#include <set>
#include <cassert>
#include <queue>

using namespace std;


namespace algorithm {

  bool allowCollisions = false;
  int moveTowardsVersion = 1;

  void setMoveTowardsVersion(int version){moveTowardsVersion = version;};
  void setAllowCollisions(bool allow){allowCollisions = allow;}; // true or false (default)

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

      moveTowards(r, make_pair(0, 0));
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
      }
    }
  }

  void Algorithm::moveTowards(state::robot &r, pair<int, int> coors) {
    if (moveTowardsVersion == 2) {
      moveTowards2(r, coors);
      return;
    }
    mission::Mission &mission = situation->mission;
    vector<int> goalPositions = mission.adjPos(coors);
    
    // Try to do a BFS-move
    for(int goalPos : goalPositions) {
      int nextPos = BFS(r.pos, goalPos);
      if(nextPos >= 0) { //Check if it worked
        r.pos = nextPos;
        return;
      }
    }
    moveAround(r);
  }

  void Algorithm::moveTowards2(state::robot &r, pair<int, int> coors) {
    mission::Mission &mission = situation->mission;
    vector<int> goalPositions = mission.adjPos(coors);
    
    int bestDist = 1e9;
    int nextPos = -1;
    // Try to do a BFS-move
    for(int goalPos : goalPositions) {
      for(int startPos : mission.adjPos(r.pos)) {
        if(occupied(startPos)) continue;
        int dist = collisionDistance(startPos, goalPos);
        if(dist != -1 && dist < bestDist) {
          nextPos = startPos;
          bestDist = dist;
        }
      }
    }
    if(nextPos != -1) {
      r.pos = nextPos;
      return;
    }

    // Otherwise do something random
    moveAround(r);
  }


  int Algorithm::BFS(int start, int destination) {
    vector<int> parent(situation->mission.positions, -1);
    queue<int> q;
    q.push(start);
    parent[start] = -3;
    while(!q.empty()) {
      int pos = q.front();
      q.pop();
      for(int neighbor : situation->mission.adjPos(pos)) {
        if(occupied(neighbor))  {
          parent[neighbor] = -2;
        }
        if(parent[neighbor] == -1) {
          q.push(neighbor);
          parent[neighbor] = pos;
        }
      }
    }
    int curpos = destination;
    while(true) {
      if(curpos < 0) {
        return -1;
      }
      if(parent[curpos] == start) {
        return curpos; //This was the next to last 
      }
      curpos = parent[curpos];
    }
    return -1;
  }

  int Algorithm::distance(int start, int destination) {
    if (moveTowardsVersion == 2)
      return collisionDistance(start, destination);
    vector<int> parent(situation->mission.positions, -1);
    queue<int> q;
    q.push(start);
    parent[start] = -3;
    while(!q.empty()) {
      int pos = q.front();
      q.pop();
      for(int neighbor : situation->mission.adjPos(pos)) {
        if(occupied(neighbor))  {
          parent[neighbor] = -2;
        }
        if(parent[neighbor] == -1) {
          q.push(neighbor);
          parent[neighbor] = pos;
        }
      }
    }

    int curpos = destination;
    int distance = 0;
    while(curpos != start) {
      distance++;
      if(curpos < 0) {
        return -1;
      }
      curpos = parent[curpos];
    }
    return distance;
  }

  int Algorithm::collisionDistance(int start, int destination) {
    vector<int> parent(situation->mission.positions, -1);
    queue<int> q;
    q.push(start);
    parent[start] = -3;
    while(!q.empty()) {
      int pos = q.front();
      q.pop();
      for(int neighbor : situation->mission.adjPos(pos)) {
        if(parent[neighbor] == -1) {
          q.push(neighbor);
          parent[neighbor] = pos;
        }
      }
    }

    int curpos = destination;
    int distance = 0;
    while(curpos != start) {
      distance++;
      if(curpos < 0) {
        return -1;
      }
      curpos = parent[curpos];
    }
    return distance;
  }


  // Try to move somewhere and be out of the way. To be called for robots that have nothing to do
  void Algorithm::moveAround(state::robot &r) {
    mission::Mission &mission = situation->mission;
    vector<int> adj = mission.adjPos(r.pos);
    if (adj.size() != 0) {
      int pos = adj[rand()%adj.size()];
      if (!occupied(pos))
        r.pos = pos;
    }
  }

  bool Algorithm::occupied(int pos) {
    if (allowCollisions) return false;
    for(state::robot r : situation->state.robots)
      if (r.pos == pos)
        return true;
    return false;
  }

  string Algorithm::name() {
    return "random";
  }
}
