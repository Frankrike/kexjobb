#include "mission.h"
#include <cassert>

namespace mission {
  Mission::Mission(){
  }

  void Mission::precalculate(){
    if (precalculated)
      return;
    precalculated = true;
    assert(positions == 0);

    width = walls.size();
    height = walls[0].size();

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

