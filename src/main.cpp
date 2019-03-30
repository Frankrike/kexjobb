#include <iostream>
#include "state.h"
#include "mission.h"
#include "situation.h"
#include "algorithm.h"
#include "pinkam1.h"
#include "pinkam2.h"
#include "vhs.h"
#include "exhaustive.h"
#include "gui.h"
using namespace std;

int main(int argc, char* argv[])
{
  Situation situation;
  string missionString;
  string temp;
  while(getline(cin, temp)) {
    missionString.append(temp);
    missionString.append("\n");
  }
  situation.mission = mission::Mission(missionString);
  situation.state = state::State(situation.mission);
  algorithm::Algorithm* algorithm;

  algorithm = new algorithm::Vhs(&situation);

  GUI gui(situation.mission);
  gui.addState(situation.state);

  cout << situation.debugView() << endl;
  int turn = 0;
  while(!situation.missionCompleted() && turn++ < 1000) {
    algorithm->makeMove();
    gui.addState(situation.state);
    cout << "--------------\nturn: " << turn
        //<< " encoded: " << situation.state.encode(situation.mission)
        << endl << situation.debugView() << endl;
  }

  cout << situation.mission.toString() << endl;

  gui.show();
  
}