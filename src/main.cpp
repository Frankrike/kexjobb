#include <iostream>
#include "state.h"
#include "mission.h"
#include "situation.h"
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
  cout << situation.debugView() << endl;
}