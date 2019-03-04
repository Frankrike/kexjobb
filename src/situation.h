#ifndef SITUATION_H_
#define SITUATION_H_

#include "mission.h"
#include "state.h"

#include <string>

class Situation {
public:
  mission::Mission mission;
  state::State state;

  string debugView();
};

#endif