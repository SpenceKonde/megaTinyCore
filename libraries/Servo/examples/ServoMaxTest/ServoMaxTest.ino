/*
  Yes, this library *really* can drive 12 servos from a single type B timer!
*/

#include <Servo.h>

Servo myservos[12];
byte pos[12];
char dir[12] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

void setup() {
  TCA0.SPLIT.CTRLA = 0;
  for (byte i = 0; i < 12; i++) {
    myservos[i].attach(i);
    pos[i] = i * 15;
  }
}

void loop() {
  for (byte i = 0; i < 12; i++) {
    myservos[i].write(pos[i]);
    pos[i] += dir[i];
    if (pos[i] == 180) {
      dir[i] = -1;
    }
    if (pos[i] == 0) {
      dir[i] = 1;
    }
  }
  delay(30);                       // waits 30ms for the servo to reach the position
}
