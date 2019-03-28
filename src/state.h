#ifndef STATE_H_
#define STATE_H_

#include "mission.h"

#include<vector>
#include<string>
using namespace std;

namespace state {
  struct robot {
    int pos; // use the corresponding mission to convert to coordinates
    int item; // held item, -1 if none
    int assignedItem; // -1 if none
    int fixedStation; // used for pinkam1
  };

  struct station {
    int order; // which order from the global queue it's currently handling, -1 if none
    vector<bool> fulfilled; // which items have been fetched
    vector<int> assignedTo; // which robot is currently fetching this item, -1 if none
  };

  // describes the state of a mission
  class State {
  private:
  public:
    vector<robot> robots;
    vector<station> stations;
    int nextOrder = 0;

    State();
    State(mission::Mission);
    State(unsigned int, mission::Mission); // decode from int
    State(string str); // read from string

    unsigned int encode(mission::Mission);
    string toString();
  };
}

#endif
