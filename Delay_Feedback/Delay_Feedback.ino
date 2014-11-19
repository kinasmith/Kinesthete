// Delay demonstration example, Teensy Audio Library
//   http://www.pjrc.com/teensy/td_libs_Audio.html
// 
// Creates a chirp on the left channel, then
// three delayed copies on the right channel.
//
// Requires the audio shield:
//   http://www.pjrc.com/store/teensy3_audio.html
//
// This example code is in the public domain.

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

// GUItool: begin automatically generated code
AudioSynthWaveform       waveform1;      //xy=66,51
AudioSynthWaveformSine   sine1;          //xy=218,109
AudioEffectEnvelope      envelope1;      //xy=353,109
AudioMixer4              mixer3;         //xy=543,157
AudioMixer4              mixer2;         //xy=545,61
AudioMixer4              mixer4;         //xy=556,232
AudioEffectDelay         delay1;         //xy=680,114
AudioMixer4              mixer1;         //xy=867,67
AudioOutputI2S           i2s1;           //xy=873,230
AudioConnection          patchCord1(sine1, envelope1);
AudioConnection          patchCord2(envelope1, 0, mixer4, 2);
AudioConnection          patchCord3(mixer3, 0, mixer2, 3);
AudioConnection          patchCord4(mixer2, delay1);
AudioConnection          patchCord5(mixer4, 0, mixer3, 3);
AudioConnection          patchCord6(delay1, 0, mixer1, 0);
AudioConnection          patchCord7(delay1, 0, mixer2, 0);
AudioConnection          patchCord8(delay1, 1, mixer2, 1);
AudioConnection          patchCord9(delay1, 2, mixer2, 2);
AudioConnection          patchCord10(delay1, 3, mixer3, 0);
AudioConnection          patchCord11(delay1, 4, mixer3, 1);
AudioConnection          patchCord12(delay1, 5, mixer3, 2);
AudioConnection          patchCord13(delay1, 6, mixer4, 0);
AudioConnection          patchCord14(delay1, 7, mixer4, 1);
AudioConnection          patchCord15(mixer1, 0, i2s1, 0);
AudioConnection          patchCord16(mixer1, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=436,481
// GUItool: end automatically generated code

float direct = 0.1;
const int ledPin = 13;

void setup() {
  pinMode(ledPin, OUTPUT);
  // allocate enough memory for the delay
  AudioMemory(128);

  // enable the audio shield
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);

  // configure a sine wave for the chirp
  // the original is turned on/off by an envelope effect
  // and output directly on the left channel
  sine1.frequency(1000);
  sine1.amplitude(0.5);
  mixer2.gain(0, direct);
  mixer2.gain(1, direct);
  mixer2.gain(2, direct);
  mixer2.gain(3, 1);
  mixer3.gain(0, direct);
  mixer3.gain(1, direct);
  mixer3.gain(2, direct);
  mixer3.gain(3, 1);
  mixer4.gain(0, direct);
  mixer4.gain(1, direct);
  mixer4.gain(2, 1);

  for(int i = 0; i < 8; i++) {
    delay1.delay(i, ((i/2)*100)/4);
  }
}

void loop() {
  sine1.frequency(random(100, 800));
  envelope1.noteOn();
 digitalWrite(ledPin, HIGH);
  delay(46);
  envelope1.noteOff();
  digitalWrite(ledPin, LOW); 
  delay(500);
}



