#include <iostream>
#include "state.h"
#include "mission.h"
#include "situation.h"
#include "algorithm.h"
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

  algorithm::Algorithm algorithm = algorithm::Algorithm(&situation);
  for(int i = 0; i < 10; i++) {
    cout << situation.debugView() << endl;
    algorithm.makeMove();
  }
  
}