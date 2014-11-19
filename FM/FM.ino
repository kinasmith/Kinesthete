#include<Encoder.h>

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
/*
// GUItool: begin automatically generated code
AudioSynthWaveformSine   sine1;          //xy=180,95
AudioSynthWaveformSineModulated sine_fm1;       //xy=326,142
AudioMixer4              mixer1;         //xy=548,175
AudioOutputI2S           i2s1;           //xy=715,176
AudioConnection          patchCord1(sine1, sine_fm1);
AudioConnection          patchCord2(sine_fm1, 0, mixer1, 0);
AudioConnection          patchCord3(mixer1, 0, i2s1, 0);
AudioConnection          patchCord4(mixer1, 0, i2s1, 1);
// GUItool: end automatically generated code
*/
// GUItool: begin automatically generated code
AudioSynthWaveformSine   sine1;          //xy=79,41
AudioSynthWaveformSineModulated sine_fm1;       //xy=223,66
AudioSynthWaveformSineModulated sine_fm2;       //xy=374,101
AudioMixer4              mixer1;         //xy=548,175
AudioOutputI2S           i2s1;           //xy=715,176
AudioConnection          patchCord1(sine1, sine_fm1);
AudioConnection          patchCord2(sine_fm1, sine_fm2);
AudioConnection          patchCord3(sine_fm2, 0, mixer1, 0);
AudioConnection          patchCord4(mixer1, 0, i2s1, 0);
AudioConnection          patchCord5(mixer1, 0, i2s1, 1);
// GUItool: end automatically generated code

AudioControlSGTL5000     audioShield;
// GUItool: end automatically generated code

Encoder leftEnc(0, 1);
Encoder rightEnc(2, 3);
Encoder pitchEnc(15,16);
long positionLeft = 0;
long positionRight = 0;
long positionPitch = 0;

float freq = 55.0;
float modMult = 0.5;

void setup(void) {
 // Serial.begin(115200);
 // Serial.println("begin");
  
  AudioMemory(8);
  audioShield.enable();
  audioShield.volume(0.5);
  //sine1.begin(0.7, freq, 0.5);
  sine1.frequency(freq);
  sine1.amplitude(0.9);
  mixer1.gain(0, 0.7);
  sine_fm1.frequency(freq*modMult);
  sine_fm2.frequency(freq);
}

void loop() {
  long newLeft, newRight, newPitch;
  newLeft = leftEnc.read();
  newRight = rightEnc.read();
  newPitch = pitchEnc.read();
  if(newLeft < 0) {
    leftEnc.write(0);
  }
  if(newLeft > 96) {
    leftEnc.write(96);
  }
  if(newRight < 0) {
    rightEnc.write(0);
  }
  if(newRight > 96) {
    rightEnc.write(96);
  }
  if(newPitch < 0) {
    pitchEnc.write(0);
  }
  if(newPitch > 4096*4) {
    pitchEnc.write(4096*4);
  }
  

  if (newLeft != positionLeft || newRight != positionRight || newPitch != positionPitch) {
   /*
    Serial.print("Left = ");
    Serial.print(newLeft);
    positionLeft = newLeft;
    Serial.print(", Right = ");
    Serial.print(newRight);
    positionRight = newRight;
    Serial.print(", Pitch = ");
   // Serial.print(newPitch);
   Serial.print(linexp(newPitch, 0, 4096*4, 55, 880));
    positionPitch = newPitch;
    Serial.println();
    */
    freq = linexp(newPitch, 0, 4096*4, 55, 880);
    modMult = linexp(newRight, 0, 96, 0.5, 4);
    AudioNoInterrupts();
    sine1.frequency(freq);
    sine_fm1.frequency(freq*modMult);
    sine_fm2.frequency(freq);
    AudioInterrupts();
  }

}

float linexp (float x, float a, float b, float c, float d)
//intput, input low, input high, output low, output high
{
  if (x <= a) return c;
  if (x >= b) return d;
  return pow(d/c, (x-a)/(b-a)) * c;
}


















