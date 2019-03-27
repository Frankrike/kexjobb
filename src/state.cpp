#include "state.h"

namespace state {
  State::State(){}

  State::State(mission::Mission mission) {
    robots.resize(mission.robots.size());
    stations.resize(mission.stations.size());
    for(int i = 0; i < int(robots.size()); i++) {
        robots[i].pos = mission.robots[i].startPos; // Currently at start position
        robots[i].item = -1; // Holding no item
        robots[i].assignedItem = -1; // Assigned no item
        robots[i].fixedStation = i;
    }
    for(int i = 0; i < int(stations.size()); i++) {
        stations[i].order = i < int(mission.orders.size()) ? i : -1;
        if(stations[i].order != -1) nextOrder = stations[i].order+1;
        if (stations[i].order != -1) {
          stations[i].fulfilled.assign(mission.orders[i].items.size(), false);
          stations[i].assignedTo.assign(mission.orders[i].items.size(), -1);
        }
    }
  }
}
