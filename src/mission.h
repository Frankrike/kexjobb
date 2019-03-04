#ifndef MISSION_H_
#define MISSION_H_

#include<vector>
#include<string>
#include<utility>
using namespace std;

namespace mission {
  struct robot {
    int startPos; 
  };

  struct station {
    pair<int, int> coors; // has to be coors, because isn't walkable space
  };

  struct order {
    vector<int> items;
  };

  struct item {
    pair<int, int> shelfCoors; // position of the shelf. Note that it's not
                               // walkable space, so it can't be a pos
                               // integer.
  };

  // describes a warehouse and its orders
  class Mission {
  private:
    vector<pair<int, int> > posToCoors; // precalculated to run toCoors and
    vector<vector<int> > coorsToPos;    // toPos quickly
    bool precalculated = false;
    void precalculate();
  public:
    vector<string> walls; // '#' if not walkable, ' ' if walkable. If occupied
                          // by a station, it's also '#'
    vector<robot> robots;
    vector<station> stations;
    vector<order> orders;
    vector<item> items;
    int width, height;
    int positions; // number of walkable squares

    Mission();
    Mission(string str); // construct from string (probably loaded from a file)

    string toString(); // the mission as a string (to be written to a file)

    void finalize(); // runs precalculations
    pair<int, int> toCoors(int pos); // only walkable space
                                     // translates to coordinates
    int toPos(pair<int, int> coors); // returns -1 if the coordinates are not at
                                     // walkable space
  };
}

#endif