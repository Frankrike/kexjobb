#include "situation.h"

#include <sstream>
#include <iostream>

string Situation::debugView(){
  stringstream s;
  vector<string> vs;
  vs.resize(mission.height, string(2*mission.width, ' '));
  for(string &str : vs)
    for(int i = 0; i < mission.width; i++)
      str[2*i] = '.';

  for(int x = 0; x < mission.width; x++)
    for(int y = 0; y < mission.height; y++)
      if (mission.toPos({x, y}) == -1) {
        vs[y][2*x] = '[';
        vs[y][2*x + 1] = ']';
      }

  for(state::robot robot : state.robots) {
    pair<int, int> coors = mission.toCoors(robot.pos);
    int x = coors.first, y = coors.second;

    vs[y][2*x] = 'R';
    vs[y][2*x + 1] = robot.item == -1 ? '_' : 'a' + robot.item;
  }

  for(int i = 0; i < int(mission.stations.size()); i++) {
    pair<int, int> coors = mission.stations[i].coors;
    int x = coors.first, y = coors.second;
    int order = state.stations[i].order;

    vs[y][2*x] = 'S';
    vs[y][2*x + 1] = order != -1 ? char('0' + order) : '_';
  }

  for(int i = 0; i < int(mission.items.size()); i++) {
    pair<int, int> coors = mission.items[i].shelfCoors;
    int x = coors.first, y = coors.second;

    vs[y][2*x] = 'I';
    vs[y][2*x + 1] = 'a' + i;
  }

  for(string str : vs)
    s << str << endl;

  s << endl;
  for(int i = 0; i < int(mission.stations.size()); i++) {
    mission::station station = mission.stations[i];
    s << "Station " << i << ": pos={" 
        << station.coors.first << "," << station.coors.second << "}";
    s << " order=" << state.stations[i].order;

    if (state.stations[i].order != -1) {
      mission::order order = mission.orders[state.stations[i].order];
      s << "(";
      for(int j = 0; j < int(order.items.size()); j++){
        if(j != 0)
          s << ',';
        s << char('a' + order.items[j]) 
            << (state.stations[i].fulfilled[j] ? 'X' : '_');
      }
      s << ")";
    }

    s << endl;
  }

  s << endl;
  for(int i = 0; i < int(mission.orders.size()); i++) {
    mission::order order = mission.orders[i];
    s << "Order " << i << ": (";
    for(int j = 0; j < int(order.items.size()); j++) {
      if(j != 0)
        s << ",";
      s << char('a' + order.items[j]);
    }
    s << ")" << endl;
  }

  s << endl;
  for(int i = 0; i < int(mission.robots.size()); i ++) {
    state::robot robot = state.robots[i];
    pair<int, int> coors = mission.toCoors(robot.pos);
    s << "Robot " << i << ": pos=" << robot.pos 
        << "{" << coors.first << "," <<  coors.second << "}"
        << " item=" << (robot.item == -1 ? '_' : char('a' + robot.item))
        << " assignedItem=" << (robot.assignedItem == -1 ? '_' : char('a' + robot.assignedItem))
        << endl;
  }

  s << endl;
  for(int i = 0; i < int(mission.items.size()); i ++) {
    mission::item item = mission.items[i];
    s << "Item " << char('a' + i) << ": shelfPos={" 
        << item.shelfCoors.first << "," <<  item.shelfCoors.second << "}"
        << endl;
  }

  return s.str();
}

bool Situation::missionCompleted(){
  bool completed = true;
  for(state::station s : state.stations)
    completed &= s.order == -1;
  return completed;
}