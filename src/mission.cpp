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
    for(int i = 0; i < int(walls.size()); i++) {
      walls[i].resize(height);
    }

    // Read wall layout
    for(int y = 0; y < height; y++) {
      string row;
      s >> row;
      for(int x = 0; x < int(row.size()); x++) {
        assert(row[x] == '.' || row[x] == '#');
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
      assert(x >= 0 && x < width);
      assert(y >= 0 && y < height);
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
      assert(x >= 0 && x < width);
      assert(y >= 0 && y < height);
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
      assert(x >= 0 && x < width);
      assert(y >= 0 && y < height);
      currentRobot.startPos = toPos(make_pair(x, y));
      robots[i] = currentRobot;
    }

    // Read the order queue
    int numOrders;
    s >> numOrders;
    orders.resize(numOrders);
    for(int i = 0; i < numOrders; i++) {
      int numItemsInOrder;
      s >> numItemsInOrder;
      order currentOrder;
      currentOrder.items.resize(numItemsInOrder);
      for(int j = 0; j < numItemsInOrder; j++) {
        int itemNumber;
        s >> itemNumber;
        assert(itemNumber >= 0);
        assert(itemNumber < numItems);
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
      walls[s.coors.first][s.coors.second] = '#';
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

    // which positions, stations, and items are adjacent to each position
    adjPosV.resize(positions);
    adjStationsV.resize(positions);
    adjItemsV.resize(positions);

    for(int pos = 0; pos < positions; pos++){
      int delta[5] = {0, 1, 0, -1, 0};
      for(int d = 0; d < 4; d++) {
        pair<int, int> coors =
            {toCoors(pos).first + delta[d], toCoors(pos).second + delta[d+1]};

        if (toPos(coors) != -1)
          adjPosV[pos].push_back(toPos(coors));
        else {
          for(int i = 0; i < int(stations.size()); i++)
            if (coors == stations[i].coors)
              adjStationsV[pos].push_back(i);
          for(int i = 0; i < int(items.size()); i++)
            if (coors == items[i].shelfCoors)
              adjItemsV[pos].push_back(i);
        }
      }
    }
  }

  pair<int, int> Mission::toCoors(int pos){
    precalculate();
    return posToCoors[pos];
  }

  int Mission::toPos(pair<int, int> coors){
    precalculate();
    if (coors.first < 0 || coors.second < 0 
        || coors.first >= width || coors.second >= height)
      return -1;
    return coorsToPos[coors.first][coors.second];
  }

  vector<int> Mission::adjPos(int pos){
    precalculate();
    return adjPosV[pos];
  }

  vector<int> Mission::adjStations(int pos){
    precalculate();
    return adjStationsV[pos];

  }
  vector<int> Mission::adjItems(int pos){
    precalculate();
    return adjItemsV[pos];
  }

  vector<int> Mission::adjPos(pair<int, int> coors) {
    vector<int> result;

    int newPos = toPos(make_pair(coors.first+1, coors.second));
    if(newPos >= 0) result.push_back(newPos);

    newPos = toPos(make_pair(coors.first-1, coors.second));
    if(newPos >= 0) result.push_back(newPos);

    newPos = toPos(make_pair(coors.first, coors.second+1));
    if(newPos >= 0) result.push_back(newPos);

    newPos = toPos(make_pair(coors.first, coors.second-1));
    if(newPos >= 0) result.push_back(newPos);

    return result;
  }

  string Mission::toString(){
    stringstream s;

    s << width << " " << height << endl;

    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++)
        s << walls[x][y];
      s << endl;
    }

    s << items.size() << endl;
    for (item i : items)
      s << i.shelfCoors.first << " " << i.shelfCoors.second << endl;

    s << stations.size() << endl;
    for (station i : stations)
      s << i.coors.first << " " << i.coors.second << endl;

    s << robots.size() << endl;
    for (robot r : robots)
      s << toCoors(r.startPos).first << " " << toCoors(r.startPos).second << endl;

    s << orders.size() << endl;
    for (order o : orders) {
      s << o.items.size();
      for (int i : o.items)
        s << " " << i;
      s << endl;
    }

    return s.str();
  }
}

