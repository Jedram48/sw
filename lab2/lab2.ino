
#include "Wheels.h"
Wheels w;

#define r_front 2
#define l_front 8
#define r_back 4
#define l_back 7
#define d 525 //

void setup() {                
  w.attach(r_front, r_back, 3, l_front, l_back, 6);
}

void loop() {

  w.setSpeed(100);
  w.forward();
}
