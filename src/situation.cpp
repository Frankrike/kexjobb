#include "situation.h"

#include <sstream>

string Situation::debugView(){
   stringstream s;
   s << "Dimensions: " << mission.width << " " << mission.height << endl;
   for(int i = 0; i < mission.walls.size(); i++) {
    s << mission.walls[i] << endl;
   }
  return s.str();
}