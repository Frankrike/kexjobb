#include "state.h"

#include <tuple>
#include <iostream>

namespace state {
  State::State(){}

  State::State(mission::Mission mission) {
    robots.resize(mission.robots.size());
    stations.resize(mission.stations.size());
    for(int i = 0; i < int(robots.size()); i++) {
        robots[i].pos = mission.robots[i].startPos; // Currently at start position
        robots[i].item = -1; // Holding no item
    }
    for(int i = 0; i < int(stations.size()); i++) {
        stations[i].order = i < int(mission.orders.size()) ? i : -1;
        if (stations[i].order != -1) {
          stations[i].fulfilled.assign(mission.orders[i].items.size(), false);
        }
    }
  }

  unsigned int put(unsigned int i, int value, int range){
    return i*range + value;
  }

  unsigned int State::encode(mission::Mission mission) {
    unsigned int encoded = 0;
    for(int r = 0; r < int(robots.size()); r++) {
      encoded = put(encoded, robots[r].pos, mission.positions); // position
      encoded = put(encoded, robots[r].item+1, mission.items.size()+1); // item
    }
    for(int s = 0; s < int(stations.size()); s++) {
      if (stations[s].order != -1) { // write items in order
        mission::order order = mission.orders[stations[s].order];
        for(int i = 0; i < int(order.items.size()); i++)
          encoded = put(encoded, stations[s].fulfilled[i], 2); // got item?
      }
      encoded = put(encoded, stations[s].order+1, mission.orders.size()+1); // order
    }
    return encoded;
  }

  pair<unsigned int, int> get(unsigned int i, int range) {
    return {i/range, i%range};
  }
  // same as encode, but backwards
  State::State(unsigned int encoded, mission::Mission mission){
    robots.resize(mission.robots.size());
    stations.resize(mission.stations.size());

    for(int s = stations.size(); s -- > 0;) {
      tie(encoded, stations[s].order) = get(encoded, mission.orders.size()+1); // order
      stations[s].order --;

      if (stations[s].order != -1) {
        mission::order order = mission.orders[stations[s].order];
        stations[s].fulfilled.resize(order.items.size());
        for(int i = order.items.size(); i -- > 0;){
          int f;
          tie(encoded, f) = get(encoded, 2); // got item?
          stations[s].fulfilled[i] = f;
        }
      }
    }

    for(int r = robots.size(); r -- > 0;) {
      tie(encoded, robots[r].item) = get(encoded, mission.items.size()+1); // item
      robots[r].item --;
      tie(encoded, robots[r].pos) = get(encoded, mission.positions); // position
    }
  }

  unsigned int State::maxEncoding(mission::Mission mission) {
    int maxOrderSize = 0;
    for (mission::order order : mission.orders)
      maxOrderSize = max(maxOrderSize, int(mission.items.size()));
    
    unsigned int encoded = 0;
    for(int r = 0; r < int(robots.size()); r++) {
      encoded = put(encoded, mission.positions-1, mission.positions); // position
      encoded = put(encoded, mission.items.size(), mission.items.size()+1); // item
    }
    for(int s = 0; s < int(stations.size()); s++) {
      for(int i = 0; i < maxOrderSize; i++)
        encoded = put(encoded, 1, 2); // item
      
      encoded = put(encoded, mission.orders.size(), mission.orders.size()+1); // order
    }
    return encoded + 1;
  }
}
