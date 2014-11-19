/* Encoder Library - TwoKnobs Example
 * http://www.pjrc.com/teensy/td_libs_Encoder.html
 *
 * This example code is in the public domain.
 */

#include <Encoder.h>

// Change these pin numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
Encoder knobLeft(0,1);
Encoder knobRight(2, 3);
Encoder pitchEnc(4,5);
//   avoid using pins with LEDs attached

void setup() {
  Serial.begin(9600);
  Serial.println("TwoKnobs Encoder Test:");
}

long positionLeft  = 0;
long positionRight = 0;
long positionPitch = 0;

void loop() {
  long newLeft, newRight, newPitch;
  newLeft = knobLeft.read();
  newRight = knobRight.read();
  newPitch = pitchEnc.read();
  if (newLeft != positionLeft || newRight != positionRight || newPitch != positionPitch) {
    Serial.print("Left = ");
    Serial.print(newLeft);
    Serial.print(", Right = ");
    Serial.print(newRight);
    Serial.print(", Pitch = ");
    Serial.print(newPitch);
    Serial.println();
    positionLeft = newLeft;
    positionRight = newRight;
    positionPitch = newPitch;
  }
  // if a character is sent from the serial monitor,
  // reset both back to zero.
  if (Serial.available()) {
    Serial.read();
    Serial.println("Reset both knobs to zero");
    knobLeft.write(0);
    knobRight.write(0);
  }
}
