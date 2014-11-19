#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include<Encoder.h>

Encoder knobLeft(0, 1);
Encoder knobRight(2, 3);
Encoder pitchEnc(4,5);

long positionLeft = 0;
long positionRight = 0;
long positionPitch = 0;
float cutOff = 800;
float Q = 0.3;
float freq = 220;

// GUItool: begin automatically generated code
AudioSynthWaveform       waveform1;      //xy=97,156
AudioSynthNoiseWhite     noise1;
AudioEffectEnvelope      envelope1;      //xy=237,157
AudioFilterBiquad        biquad1;        //xy=402,158
AudioMixer4              mixer1;         //xy=548,175
AudioOutputI2S           i2s1;           //xy=687,176
AudioConnection           patchCord1(waveform1, biquad1);
//AudioConnection          patchCord1(waveform1, envelope1);
//AudioConnection          patchCord2(envelope1, biquad1);
AudioConnection          patchCord3(biquad1, 0, mixer1, 0);
AudioConnection          patchCord4(mixer1, 0, i2s1, 0);
AudioConnection          patchCord5(mixer1, 0, i2s1, 1);
AudioConnection         pathCord6(noise1, 0, mixer1, 1);
AudioControlSGTL5000     audioShield;
// GUItool: end automatically generated code

void setup(void) {
  Serial.begin(115200);
  Serial.println("begin");
  // Set up
  AudioMemory(12);
  audioShield.enable();
  audioShield.volume(0.45); 
  audioShield.surroundSoundEnable();
  audioShield.surroundSound(7); 
  waveform1.begin(0.4, freq, WAVEFORM_SAWTOOTH);
  noise1.amplitude(0.8);
  biquad1.setLowpass(0, cutOff, Q);
  envelope1.attack(10);
  envelope1.decay(50);
  envelope1.release(250);
  mixer1.gain(0, 0.7);
  mixer1.gain(1, 0);
}


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
  cutOff = constrain(positionLeft, 0, 1000);
  cutOff = map(cutOff, 0, 1000, 200, 2000);
 // freq = newPitch;
  //freq = constrain(positionRight, 0, 1000);
 // freq = map(freq, 0, 1000, 110, 110*10);
  //Q = Q/100;

 // Serial.println(cutOff);
 // Serial.println(freq);
 // Serial.println(positionRight);
 // envelope1.noteOn();

  waveform1.frequency(freq);
  biquad1.setLowpass(0, cutOff, 0.7);
 // delay(100);
 //envelope1.noteOff();
 // delay(100);
}






