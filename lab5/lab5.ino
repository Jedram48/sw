
// #include <IRremote.h>
// #include "Wheels.h"

// #define DECODE_NEC 
// #define IR_RECEIVE_PIN 2
// IRrecv irrecv(IR_RECEIVE_PIN);
// decode_results result;

// // button 1 - 0x45
// // button 2 - 0x46
// // button 3 - 0x47
// // button OK - 0x1C
// // button forward - 0x18
// // button backward - 0x52
// // button left - 0x8
// // button right - 0x5A

// Wheels w;
// volatile char cmd;

// void setup() {
//   Serial.begin(9600);
//   irrecv.enableIRIn();
//     w.attach(5,7,8,6,2,4);
//   IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
// }

// void loop() {
//       // if (IrReceiver.decode()) {

//       //   /*
//       //    * Print a summary of received data
//       //    */
//       //   if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
//       //       Serial.println(F("Received noise or an unknown (or not yet enabled) protocol"));
//       //       // We have an unknown protocol here, print extended info
//       //       IrReceiver.printIRResultRawFormatted(&Serial, true);
//       //       IrReceiver.resume(); // Do it here, to preserve raw data for printing with printIRResultRawFormatted()
//       //   } else {
//       //       IrReceiver.resume(); // Early enable receiving of the next IR frame
//       //       IrReceiver.printIRResultShort(&Serial);
//       //       IrReceiver.printIRSendUsage(&Serial);
//       //   }
//       //   Serial.println();

//       if(irrecv.decode(&result)){
//         Serial.println(result.value);
//         switch(result.value){
//           case 0x45: w.setSpeed(100); break;
//           case 0x46: w.setSpeed(150); break;
//           case 0x47: w.setSpeed(200); break;
//           case 0x18: w.forward(); break;
//           case 0x52: w.back(); break;
//           case 0x8: w.stopRight(); w.forwardLeft(); break;
//           case 0x5A: w.stopLeft(); w.forwardRight(); break;
//           case 0x1C: w.stop(); break;
//         }
        
//     }
//     irrecv.resume();
// }

// // button 1 - 0x45
// // button 2 - 0x46
// // button 3 - 0x47
// // button OK - 0x1C
// // button forward - 0x18
// // button backward - 0x52
// // button left - 0x8
// // button right - 0x5A



#include <IRremote.h>

const int RECV_PIN = 7;
IRrecv irrecv(RECV_PIN);
decode_results results;
                                      
void setup(){
  Serial.begin(9600);
  irrecv.enableIRIn();
  irrecv.blink13(true);
}

void loop(){
  if (irrecv.decode(&results)){
        Serial.println(results.value, HEX);
        irrecv.resume();
  }
}