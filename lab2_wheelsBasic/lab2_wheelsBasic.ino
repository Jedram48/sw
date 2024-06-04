#include "Wheels.h"
#include <LiquidCrystal_I2C.h>
#include "PinChangeInterrupt.h"
#include <Servo.h>
#include <IRremote.hpp>

// piny dla sonaru (HC-SR04)
#define TRIG A2
#define ECHO A3

unsigned int distance = 0;
const int distanceToStop = 45; // cm

// RECIEVER
#define IR_RECEIVE_PIN 2 

unsigned long lastSignal = 0;

// LCD
byte LCDAddress = 0x27;

LiquidCrystal_I2C lcd(0x27,16,2); 

// WHEELS
Wheels w;

#define pinRightForward 8 // green
#define pinRightBack 7 // blue
#define pinRightSpeed 6 // yellow

#define pinLeftForward 12 // gray
#define pinLeftBack 4 // purple
#define pinLeftSpeed 5 // white

int currentSpeed = 0;
const int delayToStop = 300; // millis

// SERVO
#define SERVO 3

Servo serwo;

// Angles for servo
byte angle = 90;
byte forwardAngle = 90;
byte leftAngle = 150;
byte rightAngle = 30;


// Autopilot
bool isAuto = false;
int turnCounter = 0;
int turnCounterLimit = 3;
int backwardCounter = 0;
int backwardCounterLimit = 3;

enum class AutoPilotState {
  Stop = 'S',
  GoForward = 'F',
  ForwardBlocked = 'B',
  GoLeft = 'L',
  GoRight = 'R',
  GoBackward = 'E'
};

AutoPilotState autoPilotState;

unsigned long lastAction = 0;
unsigned long actionDelay = 250;

void setup() {
  //nie można podłączyć do 9 i 10, bo Timer1 nie działa
  w.attach(pinRightForward,pinRightBack,pinRightSpeed,
           pinLeftForward,pinLeftBack,pinLeftSpeed);
  
  Serial.println("-----------000-----------");
  
  pinMode(TRIG, OUTPUT);    // TRIG startuje sonar
  pinMode(ECHO, INPUT);     // ECHO odbiera powracający impuls
  
  serwo.attach(SERVO);
  serwo.write(angle);
  
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  
  lcd.init();
  lcd.backlight();  
  
  w.setSpeed(200); 

  Serial.begin(9600);
}

void loop() {
  if(isAuto) {
    //Serial.println("Auto");
    AutoDriving();
  }
  else {
    HumanDriving();
  }
}

void AutoDriving() {
  if(IrReceiver.decode())
  {
    Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX); // Print "old" raw data
    IrReceiver.printIRResultShort(&Serial); // Print complete received data in one line
    Serial.println("----------------------");
    switch(IrReceiver.decodedIRData.command)
    {
      case 0x1C: 
        angle = forwardAngle;
        setLCD("S");
        w.stop(); 
        isAuto = false;
        break;
    }
    Serial.println("Resume");
    lastSignal = millis();
    IrReceiver.resume();
  }

  unsigned long currentTime = millis();
  
  if(currentTime - lastAction > actionDelay) {
    lastAction = currentTime;
    Serial.println((char)autoPilotState);
    switch (autoPilotState) {
      case AutoPilotState::Stop:
        Serial.println("Stop");
        w.stop();
        break;
        
      case AutoPilotState::GoForward:
        Serial.println("GoForward");
        distance = lookAndTellDistance(forwardAngle);
        angle = forwardAngle;
        if(distance < distanceToStop) {
          w.stop();
          autoPilotState = AutoPilotState::ForwardBlocked;
          Serial.println("STOOOOOOOOOOOOOP FORWARD");
          break;
        }
        w.presetForward();
        w.forward(); 
        break;

        
      case AutoPilotState::GoLeft:
        Serial.println("GoLeft");
        distance = lookAndTellDistance(leftAngle);
        angle = leftAngle;
        if(distance < distanceToStop) {
          w.stop();
          autoPilotState = AutoPilotState::GoForward;
          Serial.println("STOOOOOOOOOOOOOP LEFT");
          break;
        }
        turnCounter++;
        if(turnCounter > turnCounterLimit) {
          turnCounter = 0;
          autoPilotState = AutoPilotState::GoForward;
        }
        w.presetLeft();
        w.goLeft(); 
        break;
        
      case AutoPilotState::GoRight:
        Serial.println("GoRight");
        distance = lookAndTellDistance(rightAngle);
        angle = rightAngle;
        if(distance < distanceToStop) {
          w.stop();
          autoPilotState = AutoPilotState::GoForward;
          Serial.println("STOOOOOOOOOOOOOP RIGHT");
          break;
        }
        turnCounter++;
        if(turnCounter > turnCounterLimit) {
          turnCounter = 0;
          autoPilotState = AutoPilotState::GoForward;
        }
        w.presetRight();
        w.goRight(); 
        break;

      case AutoPilotState::GoBackward:
        Serial.println("GoBackward");
        backwardCounter++;
        if(backwardCounter > backwardCounterLimit) {
          backwardCounter = 0;
          autoPilotState = AutoPilotState::GoForward;
        }
        w.presetBackward();
        w.back(); 
        break;
        
      case AutoPilotState::ForwardBlocked:
        Serial.println("ForwardBlocked");
        w.stop();
        unsigned int leftDistance = lookAndTellDistance(leftAngle);
        angle = leftAngle;
        Serial.println(String("Left - " + String(leftDistance)));
        delay(50);
        unsigned int rightDistance = lookAndTellDistance(rightAngle);
        angle = rightAngle;
        Serial.println(String("Right - " + String(rightDistance)));
        delay(50);
        if (leftDistance < rightDistance && rightDistance > distanceToStop) {
          Serial.println("SetRight");
          autoPilotState = AutoPilotState::GoRight;
        }
        else if(leftDistance > distanceToStop) {
          Serial.println("SetLeft");
          autoPilotState = AutoPilotState::GoLeft;
        }
        else {
          Serial.println("SetBackward");
          autoPilotState = AutoPilotState::GoBackward;
        }
        break;
        
      default:
        Serial.println("Nothing");
        break;
    }
  }
}

void HumanDriving() {
  if(IrReceiver.decode())
  {
    Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX); // Print "old" raw data
    IrReceiver.printIRResultShort(&Serial); // Print complete received data in one line
    Serial.println("----------------------");
    switch(IrReceiver.decodedIRData.command)
    {
      case 0x18: 
        setLCD("F");
        w.presetForward();
        angle = forwardAngle;
        w.forward(); 
        break;
      case 0x52: 
        setLCD("B");
        w.presetBackward();
        angle = forwardAngle;
        w.back(); 
        break;
      case 0x8: 
        w.stop();
        setLCD("L");
        //w.forwardRight(); 
        w.presetLeft();
        angle = leftAngle;
        w.goLeft(); 
        break;
      case 0x5A: 
        w.stop();
        setLCD("R");
        //w.forwardLeft(); 
        w.presetRight();
        angle = rightAngle;
        w.goRight(); 
        break;
      case 0x1C: 
        angle = forwardAngle;
        setLCD("AUTO");
        w.stop(); 
        isAuto = true;
        autoPilotState = AutoPilotState::GoForward;
        break;
      case 0x45:
        w.setSpeed(100); 
        break;
      case 0x46: 
        w.setSpeed(150); 
        break;
      case 0x47: 
        w.setSpeed(200); 
        break;
    }
    Serial.println("Resume");
    lastSignal = millis();
    IrReceiver.resume();
  }
  unsigned long currentTime = millis();
  if(currentTime - lastSignal > delayToStop){
    w.stop();
  }
  unsigned int distance = lookAndTellDistance(angle - 1);
  if(distance < distanceToStop) {
    w.stop();
    Serial.println("STOOOOOOOOOOOOOP");
  }
}

void setLCD(String _direction) {
  lcd.clear();
  lcd.setCursor(0,0);
  String s = _direction;
  s = String(s + "              ");
  s = String(s + _direction);
  lcd.print(s);
}


//ISR(PCINT1_vect) {
//  if( (PINC & (1 << PC0)) ) 
//    lc++;
//
//  if( (PINC & (1 << PC1)) )
//    rc++;
//}

unsigned int lookAndTellDistance(byte _angle) {
  
  unsigned long tot;      // czas powrotu (time-of-travel)
  unsigned int distance;

  //Serial.print("Patrzę w kącie ");
  //Serial.print(_angle);
  if(angle != _angle) {
    serwo.write(_angle);
    delay(150);
  }
  
/* uruchamia sonar (puls 10 ms na `TRIGGER')
 * oczekuje na powrotny sygnał i aktualizuje
 */
  digitalWrite(TRIG, HIGH);
  delay(20);
  digitalWrite(TRIG, LOW);
  tot = pulseIn(ECHO, HIGH);

/* prędkość dźwięku = 340m/s => 1 cm w 29 mikrosekund
 * droga tam i z powrotem, zatem:
 */
  distance = tot/58;

  //Serial.print(": widzę coś w odległości ");
  //Serial.println(distance);
  return distance;
}
