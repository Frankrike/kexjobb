#include <iostream>
#include "state.h"
#include "mission.h"
#include "situation.h"
using namespace std;

int main(int argc, char* argv[])
{
  Situation situation;
  cout << situation.debugView() << endl;
}