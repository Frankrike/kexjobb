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
  bool showGui = true;
  string algo = "vhs";
  int maxTurns = 100000;
  for (int i = 1; i < argc; i++){
    string arg(argv[i]);
    int equals = arg.find('=');
    if (equals == -1) {
      cerr << "Incorrectly formatted param \"" << arg << "\"" << endl;
      continue;
    }
    string param = arg.substr(0, equals);
    string value = arg.substr(equals+1);

    if (param == "algorithm")
      algo = value;
    else if (param == "showgui")
      showGui = value == "true";
    else if (param == "maxturns")
      maxTurns = stoi(value);
    else
      cerr << "Uknown parameter \"" << param << "\"" << endl;
  }

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

  if (algo == "vhs")
    algorithm = new algorithm::Vhs(&situation);
  else if (algo == "pinkam1")
    algorithm = new algorithm::Pinkam1(&situation);
  else if (algo == "pinkam2")
    algorithm = new algorithm::Pinkam2(&situation);
  else if (algo == "exhaustive")
    algorithm = new algorithm::Exhaustive(&situation);
  else
    algorithm = new algorithm::Algorithm(&situation);

  GUI gui(situation.mission);
  gui.setName(algorithm->name());
  gui.addState(situation.state);

  cerr << situation.debugView() << endl;
  int turn = 0;
  while(!situation.missionCompleted() && turn++ < maxTurns) {
    algorithm->makeMove();
    gui.addState(situation.state);
    cerr << "--------------\nturn: " << turn
        //<< " encoded: " << situation.state.encode(situation.mission)
        << endl << situation.debugView() << endl;
  }

  cerr << situation.mission.toString() << endl;
  cout << turn << endl;

  if (showGui)
    gui.show();
}