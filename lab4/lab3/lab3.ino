#include "Wheels.h"

Wheels w;

void setup()
{
  w.setTimer();
  w.attach(2, 4, 3, 7, 8, 5);
}

void loop()
{
  w.setSpeed(100);
  w.back();
}