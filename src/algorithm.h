#ifndef ALGORITHM_H_
#define ALGORITHM_H_

#include "situation.h"

namespace algorithm {

class Algorithm {
private:
    Situation* situation;
public:
    Algorithm(Situation* s);  
    void makeMove();

};

class Pinkam1 : public Algorithm  {
public:
    void makeMove(); 
};

}
#endif