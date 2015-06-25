/* Encoder Library - Basic Example
 * http://www.pjrc.com/teensy/td_libs_Encoder.html
 *
 * This example code is in the public domain.
 */

#include <Encoder.h>

// Change these two numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
Encoder myEnc1(5, 6);
Encoder myEnc2(7, 8);
//   avoid using pins with LEDs attached

void setup() {
  Serial.begin(9600);
  Serial.println("Basic Encoder Test:");
}

long oldPosition1  = 0;
long oldPosition2 = 0;

void loop() {
  long newPosition1 = myEnc1.read();
  if (newPosition1 != oldPosition1) {
    oldPosition1 = newPosition1;
    Serial.println("enc1: " + newPosition1);
  }
  long newPosition2 = myEnc2.read();
  if(newPosition2 != oldPosition2) {
    oldPosition2 = newPosition2;
    Serial.println("enc2: " + newPosition2);
  }
}
