#include "mission.h"

#include <cassert>
#include <sstream>
#include <iostream>

namespace mission {
  Mission::Mission(){
  }

  Mission::Mission(string str) {
    stringstream s(str);

    // Read warehouse dimensions
    s >> width;
    s >> height;

    walls.resize(width);
    for(int i = 0; i < walls.size(); i++) {
      walls[i].resize(height);
    }

    // Read wall layout
    for(int y = 0; y < height; y++) {
      string row;
      s >> row;
      for(int x = 0; x < row.size(); x++) {
        walls[x][y] = row[x];
      }
    }

    // Read the items
    int numItems;
    s >> numItems;
    items.resize(numItems);
    for(int i = 0; i < numItems; i++) {
      item currentItem;
      int x, y;
      s >> x >> y;
      currentItem.shelfCoors.first = x;
      currentItem.shelfCoors.second = y;
      items[i] = currentItem;
    }

    // Read the stations
    int numStations;
    s >> numStations;
    stations.resize(numStations);
    for(int i = 0; i < numStations; i++) {
      station currentStation;
      int x, y;
      s >> x >> y;
      currentStation.coors.first = x;
      currentStation.coors.second = y;
      stations[i] = currentStation;
    }


    // Now we know the warehouse layout, and have to run precalculations
    // to know positions for robots
    precalculate();


    // Read the robots
    int numRobots;
    s >> numRobots;
    robots.resize(numRobots);
    for(int i = 0; i < numRobots; i++) {
      robot currentRobot;
      int x, y;
      s >> x >> y;
      currentRobot.startPos = toPos(make_pair(x, y));
      robots[i] = currentRobot;
    }

    // Read the order queue
    int numOrders;
    s >> numOrders;
    orders.resize(numOrders);
    for(int i = 0; i < numOrders; i++) {
      int numItems;
      s >> numItems;
      order currentOrder;
      currentOrder.items.resize(numItems);
      for(int j = 0; j < numItems; j++) {
        int itemNumber;
        s >> itemNumber;
        currentOrder.items[j] = itemNumber;
      }
      orders[i] = currentOrder;
    }

  }

  void Mission::precalculate(){
    if (precalculated)
      return;
    precalculated = true;
    assert(positions == 0);

    // stations and shelves are not walkable
    for(station s : stations)
      walls[s.coors.first][s.coors.second] = 'S';
    for(item i : items)
      walls[i.shelfCoors.first][i.shelfCoors.second] = '#';

    coorsToPos = vector<vector<int> >(width, vector<int>(height, -1));
    for(int x = 0; x < width; x++)
      for(int y = 0; y < height; y++)
        if (walls[x][y] == '.') {
          coorsToPos[x][y] = positions;
          posToCoors.push_back({x, y});
          positions ++;
        }
  }

  pair<int, int> Mission::toCoors(int pos){
    precalculate();
    return posToCoors[pos];
  }

  int Mission::toPos(pair<int, int> coors){
    precalculate();
    return coorsToPos[coors.first][coors.second];
  }
}

