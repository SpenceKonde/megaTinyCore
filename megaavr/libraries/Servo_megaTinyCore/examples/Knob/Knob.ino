/*
  Controlling a servo position using a potentiometer (variable resistor)
  by Michal Rinott <http://people.interaction-ivrea.it/m.rinott>

  modified on 8 Nov 2013
  by Scott Fitzgerald
  http://www.arduino.cc/en/Tutorial/Knob
*/

#include <Servo_megaTinyCore.h>

Servo myservo;  // create servo object to control a servo

int potpin = PIN_PA7;  // analog pin used to connect the potentiometer - analog channel 7 exists everywhere!
int val;    // variable to read the value from the analog pin

void setup() {
  myservo.attach(PIN_PA1);  // attaches the servo on PA1, this pin exists on all tinyAVRs.
}

void loop() {
  val = analogRead(potpin);            // reads the value of the potentiometer (value between 0 and 1023)
  val = map(val, 0, 1023, 0, 180);     // scale it to use it with the servo (value between 0 and 180)
  myservo.write(val);                  // sets the servo position according to the scaled value
  delay(15);                           // waits for the servo to get there
}
