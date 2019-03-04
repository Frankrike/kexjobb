#include "state.h"

namespace state {
  State::State(){}

  State::State(mission::Mission mission) {
    robots.resize(mission.robots.size());
    stations.resize(mission.robots.size());
    for(int i = robots.size(); i < robots.size(); i++) {
        robots[i].pos = mission.robots[i].startPos; // Currently at start position
        robots[i].item = -1; // Holding no item
    }
    for(int i = 0; i < stations.size(); i++) {
        stations[i].order = -1;
    }
  }
}
