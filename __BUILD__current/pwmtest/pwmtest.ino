//*************************************************
//expects <encoder number(0-3)> <color (r b g)> <value(0-4095 (4095 is off))>.

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
// you can also call it with a different address you want
//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x41);

int currentColor = 0;
int currentEnc = 0;


void setup() {
  Serial.begin(9600);
 // Serial.println("16 channel PWM test!");
  pwm.begin();
  pwm.setPWMFreq(1600);  // This is the maximum PWM frequency

  // save I2C bitrate
  uint8_t twbrbackup = TWBR;
  // must be changed after calling Wire.begin() (inside pwm.begin())
  TWBR = 12; // upgrade to 400KHz!
  for(uint8_t i = 0; i < 16; i++){
    pwm.setPWM(i, 0, 4095);
  }
}

String comm;

void loop() {
  if (Serial.available() ) {
    char c = Serial.read();
    if(c == '.') {
      parseCommand(comm);
      comm = "";
    } 
    else {
      comm += c;
    }
  }
}

void parseCommand(String com) {
  int enc;
  String color;
  String theRest;
  int val;
  enc = com.substring(0, com.indexOf(" ")).toInt();
  theRest = com.substring(com.indexOf(" ") + 1);
  color = theRest.substring(0, theRest.indexOf(" "));
  val = theRest.substring(theRest.indexOf(" ") + 1).toInt();

  switch (enc) {
  case 0:
    switchColor(enc, color, val);
    break;
  case 1:
    switchColor(enc+2, color, val);
    break;
  case 2:
    switchColor(enc+4, color, val);
    break;
  case 3:
    switchColor(enc+6, color, val);
    break;
  }
}

void switchColor(int encoder, String col, int value) {
  if(col.equalsIgnoreCase("r")) {
    pwm.setPWM(encoder, 0, value);
  } 
  else if(col.equalsIgnoreCase("g")) {
    pwm.setPWM(encoder+1, 0, value);
  } 
  else if(col.equalsIgnoreCase("b")) {
    pwm.setPWM(encoder+2, 0, value);
  }  
}















