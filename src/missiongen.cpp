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

    for (int i = 0; i < orders; i++) {
      mission.orders[i].items.resize(1 + rand()%ordersize);
      for(int &k : mission.orders[i].items)
        k = rand()%items;
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
  }

  mission.precalculate();
  for (int i = 0; i < robots; i++)
    mission.robots[i].startPos = mission.toPos(startCoors[i]);
  cout << mission.toString();
}