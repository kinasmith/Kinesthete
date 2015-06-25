#include <Encoder.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

// GUItool: begin automatically generated code
AudioSynthWaveformSine   sine1;          //xy=116,85.75
AudioSynthWaveform       waveform1;      //xy=166,266
AudioMixer4              mixer1;         //xy=242,108.5
AudioFilterStateVariable filter1;        //xy=328,270
AudioSynthWaveformSineModulated sine_fm1;       //xy=376.25,106.5
AudioMixer4              mixer2;         //xy=578,111
AudioMixer4              mixer3;         //xy=578,111
AudioOutputI2S           i2s1;           //xy=736,223.3333282470703
AudioConnection          patchCord1(sine1, 0, mixer1, 0);
AudioConnection          patchCord2(waveform1, 0, filter1, 0);
AudioConnection          patchCord3(mixer1, sine_fm1);
AudioConnection          patchCord4(filter1, 0, mixer2, 1);
AudioConnection          patchCord5(sine_fm1, 0, mixer2, 0);
AudioConnection          patchCord6(mixer2, 0, mixer3, 0);
AudioConnection          patchCord7(mixer2, 0, mixer3, 1);
AudioConnection          patchCord8(mixer3, 0, i2s1, 0);
AudioConnection          patchCord9(mixer3, 0, i2s1, 1);
// GUItool: end automatically generated code


AudioControlSGTL5000     audioShield;
// GUItool: end automatically generated code


Encoder leftEnc(0, 1);
long positionLeft = 0;
Encoder rightEnc(2, 3);
long positionRight = 0;
Encoder pitchEnc(4,5);
long positionPitch = 0;

float freq = 55.0;
float f = 220;
float q = 2;
float knob1 = 0.5;
float knob2 = 0.5;

float fade = 0.9;

int rot = 4;

int pStrip1Pin = A1;
int pStrip2Pin = A2;
int pStrip3Pin = A3;
int ampPin = A7;

float pStrip1, pStrip2, pStrip3; 
float amp = 0.1;
float modIndex;

long prevMillis = 0;
long inter = 1000;

void setup(void) {
  AudioMemory(8);
  audioShield.enable();
  audioShield.volume(0.6);

  sine1.frequency(freq);
  sine1.amplitude(0.3);
  mixer1.gain(0, 5);
  sine_fm1.frequency(freq);
  sine_fm1.amplitude(0.4);

  waveform1.begin(0.3, freq, WAVEFORM_TRIANGLE);
  filter1.frequency(f);
  filter1.octaveControl(2.5);
  filter1.resonance(q);
  mixer2.gain(0, fade);
  mixer2.gain(1, abs(fade-1));
}

void loop() {
  unsigned long currentMillis = millis();

  pStrip1 = analogRead(pStrip1Pin); // 1 to 900
  pStrip2 = analogRead(pStrip2Pin); //100 to 900
  pStrip3 = analogRead(pStrip3Pin); //1 to 900
  amp = analogRead(ampPin); //1 to 900

  long newLeft, newRight, newPitch;
  newLeft = leftEnc.read();
  newRight = rightEnc.read();
  newPitch = pitchEnc.read();
  if(newLeft < 0) {
    leftEnc.write(0);
  }
  if(newLeft > 96*rot) {
    leftEnc.write(96*rot);
  }
  if(newRight < 0) {
    rightEnc.write(0);
  }
  if(newRight > 96*rot) {
    rightEnc.write(96*rot);
  }
  if(newPitch < 0) {
    pitchEnc.write(0);
  }
  if(newPitch > 4096*10) {
    pitchEnc.write(4096*10);
  }
  f = linexp(pStrip2, 100, 900, 1.5, 10);
  amp = constrain(linexp(amp, 100, 800, 0.01, 1) - 0.05, 0, 1);

  if (newLeft != positionLeft || newRight != positionRight || newPitch != positionPitch) {
    positionLeft = newLeft;
    positionRight = newRight;
    positionPitch = newPitch;
    freq = linexp(positionPitch, 0, 4096*10, 40, 2000);
    fade = linlin(positionLeft, 0, 96*rot, 0.01, 1);
    modIndex = linlin(positionRight, 0, (96*rot)*2, 1, 2);
    mixer2.gain(0, fade);
    mixer2.gain(1, abs(fade-1));

  }

  AudioInterrupts();
  waveform1.frequency(freq);
  sine_fm1.frequency(freq);
  sine1.frequency(freq * modIndex);
  filter1.frequency(freq*f);
  mixer3.gain(0, amp);
  mixer3.gain(1, amp);
  AudioNoInterrupts();
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






