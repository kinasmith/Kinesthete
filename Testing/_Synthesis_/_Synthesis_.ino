#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

// GUItool: begin automatically generated code
AudioSynthWaveform       waveform1;      //xy=66,51
AudioSynthWaveformSine   sine1;          //xy=247,148
AudioSynthWaveform       waveform2;      //xy=250,75
AudioEffectMultiply      multiply1;      //xy=429,105
AudioSynthWaveformSineModulated sine_fm1;       //xy=520,265
AudioMixer4              mixer2;         //xy=599,91
AudioMixer4              mixer1;         //xy=659,280
AudioOutputI2S           i2s1;           //xy=816,279
AudioConnection          patchCord1(sine1, 0, multiply1, 1);
AudioConnection          patchCord2(waveform2, 0, multiply1, 0);
AudioConnection          patchCord3(waveform2, 0, mixer2, 0);
AudioConnection          patchCord4(multiply1, 0, mixer2, 1);
AudioConnection          patchCord5(sine_fm1, 0, mixer1, 0);
AudioConnection          patchCord6(mixer2, sine_fm1);
AudioConnection          patchCord7(mixer1, 0, i2s1, 0);
AudioConnection          patchCord8(mixer1, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=854,458
// GUItool: end automatically generated code

