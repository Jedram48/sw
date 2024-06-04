#include "Wheels.h"

#include "PinChangeInterrupt.h"

#define INTINPUT0 A0
#define INTINPUT1 A1

Wheels w;
volatile int cnt0, cnt1;
volatile char cmd;

void setup() {
  w.attach(2, 4, 3, 7, 8, 5);
  w.setSpeed(180);

  // Serial.begin(9600);
  // Serial.println("Forward: WAD");
  // Serial.println("Back: ZXC");
  // Serial.println("Stop: S");

  w.forward();

  Serial.begin(9600);

  pinMode(INTINPUT0, INPUT);
  pinMode(INTINPUT1, INPUT);

  cnt0=0;
  cnt1=0;

  PCICR  = 0x02;  // włącz pin change interrupt dla 1 grupy (A0..A5)
  PCMSK1 = 0x03;  // włącz przerwanie dla A0, A1

}

void loop() {
  //   while(Serial.available())
  // {
  //   cmd = Serial.read();
  //   switch(cmd)
  //   {
  //     case 'w': w.forward(); break;
  //     case 'x': w.back(); break;
  //     case 'a': w.forwardLeft(); break;
  //     case 'd': w.forwardRight(); break;
  //     case 'z': w.backLeft(); break;
  //     case 'c': w.backRight(); break;
  //     case 's': w.stop(); break;
  //     case '1': w.setSpeedLeft(75); break;
  //     case '2': w.setSpeedLeft(200); break;
  //     case '9': w.setSpeedRight(75); break;
  //     case '0': w.setSpeedRight(200); break;
  //     case '5': w.setSpeed(100); break;
  //   }
  // }
  Serial.print(cnt0);
  Serial.print(" ");
  Serial.println(cnt1);
  delay(250);
}

ISR(PCINT1_vect) {
  if(digitalRead(INTINPUT0))
    cnt0++;
  if(digitalRead(INTINPUT1))
    cnt1++;
}
