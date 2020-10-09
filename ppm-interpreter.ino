#include "PPMReader.h"
// #include <InterruptHandler.h>   <-- You may need this on some versions of Arduino

#define GAMEPAD_OUTPUT
//#define RC_OUTPUT

// Initialize a PPMReader on digital pin 3 with 6 expected channels.
int interruptPin = 3;
int channelAmount = 8;
PPMReader ppm(interruptPin, channelAmount);

// Radiolink TS8 Channels
// 1  RX
// 2  RY
// 3  LY
// 4  LX
// 5  RT
// 6  RB
// 7  LT
// 8  LS

#ifdef RC_OUTPUT
#include <Servo.h>
int rcPins[8] = {4, 5, 6, 7, 8, 9, 10, 11};
Servo rcOutput[8] = {Servo(), Servo(), Servo(), Servo(), Servo(), Servo(), Servo(), Servo() };
#endif

#ifdef GAMEPAD_OUTPUT 
#include "Gamepad.h"
Gamepad gp(true);
#endif

void setup() {
    #ifdef RC_OUTPUT
    Serial.begin(115200);
    for(int i=0; i < channelAmount; i++){
      pinMode(rcPins[i], OUTPUT);
      rcOutput[i].attach(rcPins[i]);
    }
    #endif
}

int ppmToPWM(int ppmVal){
  // ppm is 1000-2000
  return constrain((ppmVal - 1000) / 4, 0, 255);
}

int ppmToAxis(int ppmVal){
  return constrain(((ppmVal - 1000) / 4)-127, -127, 127);
}

void loop() {
    int lx, ly, rx, ry;
    
    // Print latest valid values from all channels
    for (int channel = 1; channel <= channelAmount; ++channel) {
        unsigned long value = ppm.latestValidChannelValue(channel, 0);

        #ifdef GAMEPAD_OUTPUT
        if(channel == 1) {gp.setRightXaxis(ppmToAxis(value));}
        else if(channel == 2) {gp.setRightYaxis(ppmToAxis(value));}
        else if(channel == 3) {gp.setLeftYaxis(ppmToAxis(value));}
        else if(channel == 4) {gp.setLeftXaxis(ppmToAxis(value));}
        else if(channel == 5) {
          gp.setButtonState(0, ppmToAxis(value) >= 50);
          gp.setButtonState(1, ppmToAxis(value) <= -50);
        }
        else if(channel == 6) { gp.setButtonState(2, ppmToAxis(value) >= 50); }
        else if(channel == 7) {
          gp.setButtonState(3, ppmToAxis(value) >= 50);
          gp.setButtonState(4, ppmToAxis(value) <= -50);
        }
        else if(channel == 8) { gp.setButtonState(5, ppmToAxis(value) >= 50); }
        #endif
        
        #ifdef RC_OUTPUT
        rcOutput[channel-1].writeMicroseconds(value);
//        analogWrite(pwmPins[channel-1], ppmToPWM(value));
        Serial.print(String(value) + "    ");
//        Serial.print(" " + String(ppmToPWM(value)) + "    ");
        #endif
    }
    // 8 is scroll wheel
    // 3 is LY
    // 4 is LX
    
    Serial.println();
}
