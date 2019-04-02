#include "missiongen.h"
#include <iostream>
#include <string>
#include <set>

using namespace std;
using namespace mission;


int main(int argc, char** argv) {

  int width = 10, 
      height = 10,
      robots = 1, 
      stations = 1, 
      orders = 1, 
      ordersize = 1,
      items = 1, 
      seed = 0;
  double density = 0;

  int shelvesX = 1,
      shelvesY = 1,
      margin = 2,
      shelfLength = 4,
      marginStations = -1; 

  string mode = "random";

  for (int i = 1; i < argc; i++){
    string arg(argv[i]);
    int equals = arg.find('=');
    if (equals == -1) {
      cout << "Incorrectly formatted param \"" << arg << "\"" << endl;
      continue;
    }
    string param = arg.substr(0, equals);
    string value = arg.substr(equals+1);

    if (param == "w")
      width = stoi(value);
    else if (param == "h")
      height = stoi(value);
    else if (param == "robots")
      robots = stoi(value);
    else if (param == "stations")
      stations = stoi(value);
    else if (param == "orders")
      orders = stoi(value);
    else if (param == "ordersize")
      ordersize = stoi(value);
    else if (param == "items")
      items = stoi(value);
    else if (param == "seed")
      seed = stoi(value);
    else if (param == "density")
      density = stod(value);

    else if (param == "shelvesX")
      shelvesX = stod(value);
    else if (param == "shelvesY")
      shelvesY = stod(value);
    else if (param == "margin")
      margin = stod(value);
    else if (param == "shelfLength")
      shelfLength = stod(value);
    else if (param == "marginStations")
      marginStations = stod(value);

    else if (param == "mode")
      mode = value;
    else
      cout << "Uknown parameter \"" << param << "\"" << endl;
  }

  srand(seed);

  Mission mission;

  mission.width = width, mission.height = height;
  mission.walls = vector<string>(width, string(height, '.'));
  mission.stations.resize(stations);
  mission.robots.resize(robots);
  mission.items.resize(items);
  mission.orders.resize(orders);
  vector<pair<int, int>> startCoors(robots);

  if (mode == "random") {
    for (int x = 0; x < width; x ++)
      for (int y = 0; y < height; y ++)
        if ((rand()%1000)/1000.0 < density)
          mission.walls[x][y] = '#';

    auto randCoors = [&](){
      int n = width*height;
      for (int p = rand()%n;; p = (p+1)%n) {
        int x = p/width, y = p%width; 
        if (mission.walls[x][y] == '.')
          return pair<int, int>(x, y);
      }
      return pair<int, int>(0, 0);
    };

    for (int i = 0; i < stations; i++) {
      pair<int, int> coors = randCoors();
      mission.walls[coors.first][coors.second] = '#';
      mission.stations[i].coors = coors;
    }

    for (int i = 0; i < items; i++) {
      pair<int, int> coors = randCoors();
      mission.walls[coors.first][coors.second] = '#';
      mission.items[i].shelfCoors = coors;
    }

    set<pair<int, int> > used;
    for (int i = 0; i < robots; i++) {
      pair<int, int> coors = randCoors();
      while(used.count(coors)){
        coors = randCoors();
      }
      used.insert(coors);
      startCoors[i] = coors;
    }
  } else if (mode == "shelves") {

    if (marginStations == -1)
      marginStations = margin;

    int shelfTotLength = shelfLength + 2;
    width = shelvesX*(shelfTotLength + margin) + marginStations + 1;
    height = max(stations, max(robots, margin + shelvesY*(2 + margin)));
    items = shelfLength * 2 * shelvesX * shelvesY;

    mission.items.resize(items);
    mission.width = width;
    mission.height = height;
    mission.walls = vector<string>(width, string(height, '.'));

    for (int s = 0; s < stations; s++) {
      int x = 0, y = (height-stations)/2 + s;
      mission.stations[s].coors = {x, y};
    }
    for (int r = 0; r < robots; r++) {
      int x = 1, y = (height-robots)/2 + r;
      startCoors[r] = {x, y};
    }

    int shelvesStartX = 1 + marginStations; 

    int itemId = 0;
    for (int sx = 0; sx < shelvesX; sx ++)
      for (int sy = 0; sy < shelvesY; sy ++) {
        int x1 = shelvesStartX + sx*(shelfTotLength + margin);
        int y1 = margin + sy*(2 + margin);
        mission.walls[x1][y1] = mission.walls[x1][y1+1]
            = mission.walls[x1+shelfLength+1][y1]
            = mission.walls[x1+shelfLength+1][y1+1]
            = '#';
        for (int x = x1 + 1; x <= x1 + shelfLength; x++) {
          mission.items[itemId++].shelfCoors = {x, y1};
          mission.items[itemId++].shelfCoors = {x, y1+1};
        }
      }
  } else cout << "Uknown mode \"" << mode << "\"." << endl;

  for (int i = 0; i < orders; i++) {
    mission.orders[i].items.resize(1 + rand()%ordersize);
    for(int &k : mission.orders[i].items)
      k = rand()%items;
  }

  mission.precalculate();
  for (int i = 0; i < robots; i++)
    mission.robots[i].startPos = mission.toPos(startCoors[i]);
  cout << mission.toString();
}