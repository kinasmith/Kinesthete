#include <Encoder.h>


// demonstrate pulse with slow changes in pulse width

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

// GUItool: begin automatically generated code
AudioSynthWaveform       waveform1;      //xy=188,240
AudioSynthWaveformSine   sine1;          //xy=116,85.75


AudioEffectEnvelope      envelope1;      //xy=371,237
AudioFilterStateVariable filter1;        //xy=234,281
AudioOutputI2S           i2s1;           //xy=565,241
AudioConnection          patchCord1(waveform1, envelope1);
AudioConnection          patchCord2(envelope1, 0, filter1, 0);

AudioConnection          patchCord3(filter1, 0, i2s1, 0);
AudioConnection          patchCord4(filter1, 0, i2s1, 1);

AudioConnection          patchCord5(sine1, 0, filter1, 1);

AudioControlSGTL5000     audioShield;     //xy=586,175
// GUItool: end automatically generated code

Encoder leftEnc(0, 1);
long positionLeft = 0;
Encoder rightEnc(2, 3);
long positionRight = 0;
Encoder pitchEnc(15,16);
long positionPitch = 0;

float freq = 55.0;
float f = 220;

void setup(void)
{

  // Set up
  AudioMemory(8);
  audioShield.enable();
  audioShield.volume(0.45);

  sine1.frequency(2);

  waveform1.pulseWidth(0.5);
  waveform1.begin(0.5, 220, WAVEFORM_SAWTOOTH);
  filter1.frequency(f);
  filter1.octaveControl(2.5);

  envelope1.attack(50);
  envelope1.decay(50);
  envelope1.release(250);

  envelope1.noteOn();

}

void loop() {
  float w;
  float q;

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
    positionLeft = newLeft;
    positionRight = newRight;
    positionPitch = newPitch;
    //w = linlin(positionLeft, 0, 96*4, 0.01, 1);
    // waveform1.pulseWidth(w);
    q = linlin(positionLeft, 0, 96*4, 0.7, 5);
    freq = linexp(positionPitch, 0, 4096*4, 55, 880);
    f = linexp(positionRight, 0, 96, 1, 10);
    
    filter1.resonance(q);
    filter1.frequency(freq*f);

    AudioNoInterrupts();
    waveform1.frequency(freq);
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












