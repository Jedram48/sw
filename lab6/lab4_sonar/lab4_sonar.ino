#include <LiquidCrystal_I2C.h>

#include <Servo.h>

// piny dla sonaru (HC-SR04)
#define TRIG A4
#define ECHO A5

// pin kontroli serwo (musi być PWM)
#define SERVO 3

Servo serwo;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  pinMode(TRIG, OUTPUT);    // TRIG startuje sonar
  pinMode(ECHO, INPUT);     // ECHO odbiera powracający impuls

  lcd.init(); 
  lcd.backlight(); 

  Serial.begin(9600);

  serwo.attach(SERVO);

/* rozejrzyj się w zakresie od 0 stopni (patrz na jedną burtę)
 *  do 180 stopni (patrz na prawą burtę). Wydrukuj na konsoli
 *  kierunek patrzenia i najbliżej widziany obiekt (pojedynczy pomiar)
 */
  for(byte angle = 0; angle < 180; angle+= 20) {
    lookAndTellDistance(angle);
    delay(500);
  }
  
/* patrz przed siebie */
  serwo.write(90);

}

// void loop() { /* nic nie rób */ }
void loop()
{
    // Clear the display buffer
    lcd.clear(); 
    // Set cursor (Column, Row)
    lcd.setCursor(0, 0);
    // print "Hello" at (0, 0)        
    lcd.print("Hello"); 
    // Set cursor (Column, Row)
    lcd.setCursor(0,1);
    // print "Geek" at (0, 1)
    lcd.print("Geek");
 
    delay(100);
}

void lookAndTellDistance(byte angle) {
  
  unsigned long tot;      // czas powrotu (time-of-travel)
  unsigned int distance;

  Serial.print("Patrzę w kącie ");
  Serial.print(angle);
  serwo.write(angle);
  
/* uruchamia sonar (puls 10 ms na `TRIGGER')
 * oczekuje na powrotny sygnał i aktualizuje
 */
  digitalWrite(TRIG, HIGH);
  delay(10);
  digitalWrite(TRIG, LOW);
  tot = pulseIn(ECHO, HIGH);

/* prędkość dźwięku = 340m/s => 1 cm w 29 mikrosekund
 * droga tam i z powrotem, zatem:
 */
  distance = tot/58;

  Serial.print(": widzę coś w odległości ");
  Serial.println(distance);
}
