#include<Encoder.h>

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

// GUItool: begin automatically generated code
AudioSynthWaveform       waveform1;      //xy=66,51
AudioSynthWaveformSineModulated sine_fm1;       //xy=204,54
AudioMixer4              mixer2;         //xy=315,197
AudioSynthWaveformSineModulated sine_fm2;       //xy=366,71
AudioFilterStateVariable filter1;        //xy=475,153
AudioMixer4              mixer1;         //xy=611,157
AudioOutputI2S           i2s1;           //xy=748,166
AudioConnection          patchCord1(waveform1, sine_fm1);
AudioConnection          patchCord2(waveform1, 0, mixer2, 1);
AudioConnection          patchCord3(sine_fm1, sine_fm2);
AudioConnection          patchCord4(mixer2, 0, filter1, 0);
AudioConnection          patchCord5(sine_fm2, 0, mixer2, 0);
AudioConnection          patchCord6(filter1, 0, mixer1, 0);
AudioConnection          patchCord7(filter1, 1, mixer1, 1);
AudioConnection          patchCord8(filter1, 2, mixer1, 2);
AudioConnection          patchCord9(mixer1, 0, i2s1, 0);
AudioConnection          patchCord10(mixer1, 0, i2s1, 1);
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
float amp = 0;
float filt = 3.0;

void setup(void) {
 // Serial.begin(115200);
 // Serial.println("begin");
  
  AudioMemory(8);
  audioShield.enable();
  audioShield.volume(0.5);
  waveform1.begin(0.7, freq, WAVEFORM_SAWTOOTH);
  mixer1.gain(0, 0.6);
  mixer1.gain(1, 0);
  mixer1.gain(2, 0);
  mixer1.gain(3, 0.7);
  sine_fm1.frequency(freq*modMult);
  sine_fm2.frequency(freq);
  filter1.frequency(freq);
  filter1.resonance(0.2);
}

void loop() {
  long newLeft, newRight, newPitch;
  newLeft = leftEnc.read();
  newRight = rightEnc.read();
  newPitch = pitchEnc.read();
  if(newLeft < 0) {
    leftEnc.write(0);
  }
  if(newLeft > 96*4) {
    leftEnc.write(96*4);
  }
  if(newRight < 0) {
    rightEnc.write(0);
  }
  if(newRight > 96*4) {
    rightEnc.write(96*4);
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
    modMult = linexp(newRight, 0, 96*4, 0.5, 4);
    amp = linlin(newLeft, 0, 96*4, 0.0, 1.0);
    AudioNoInterrupts();
    waveform1.frequency(freq);
    waveform1.amplitude(amp);
    sine_fm1.frequency(freq*modMult);
    sine_fm2.frequency(freq);
    filter1.frequency(freq*2);
    filter1.resonance(filt);
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

float linlin (float x, float a, float b, float c, float d)
{
  if (x <= a) return c;
  if (x >= b) return d;
  return (x - a) / (b - a) * (d - c) + c;
}














