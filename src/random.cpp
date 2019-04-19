#include "random.h"
#include <cmath>

long long int seed = 0;
const long long int P = 1103515245, Q = 12345;

int rand() {
  seed = P*seed + Q;
  return std::abs((int)(seed));
}

void srand(unsigned int s){
  seed = s;
}