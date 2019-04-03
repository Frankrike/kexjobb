#ifndef GUI_H_
#define GUI_H_

#include <vector>
#include "state.h"
#include "mission.h"

class GUI {
private:
  vector<state::State> states;
  mission::Mission mission;
  string name = "";
public:
  GUI(mission::Mission mission);
  void addState(state::State state);
  void setName(string name);
  void show();
};

#endif