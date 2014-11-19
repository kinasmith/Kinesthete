//INIT Libraries
#include <Encoder.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

//Create UGen's
AudioSynthWaveformSine   Modulator;
AudioSynthWaveform       Wave;
AudioMixer4              FMattn;
AudioFilterStateVariable Filt;
AudioSynthWaveformSineModulated Carrier;
AudioMixer4              Mix;
AudioSynthWaveformDc     EnvCtrl;
AudioEffectMultiply      VCA;
AudioOutputI2S           Out; 
//Delay Ugens and Patchcords
AudioEffectDelay         Delay;
AudioMixer4              delayMix1;
AudioMixer4              delayMix2;
AudioMixer4              delayMix3;
//Make patch Connections
AudioConnection          patchCord1(Modulator, 0, FMattn, 0);
AudioConnection          patchCord2(FMattn, 0, Carrier, 0);
AudioConnection          patchCord3(Carrier, 0, Mix, 0);
AudioConnection          patchCord4(Wave, 0, Filt, 0);
AudioConnection          patchCord5(Filt, 0, Mix, 1);
AudioConnection          patchCord6(EnvCtrl, 0, VCA, 1);
AudioConnection          patchCord7(Mix, 0, VCA, 0);
AudioConnection          TestCord1(Modulator, 0, Out, 0);
AudioConnection          TestCord2(Modulator, 0, Out, 1);

//AudioConnection          patchCord20(VCA, 0, Out, 0);
//AudioConnection          patchCord21(VCA, 0, Out, 1);

//Delay Connections
AudioConnection          patchInput(VCA, 0, delayMix1, 0); //audio input
AudioConnection          patchSignal1(delayMix1, 0, delayMix2, 0);
AudioConnection          patchSignal2(delayMix2, 0, delayMix3, 0);
AudioConnection          patchSignal3(delayMix3, 0, Delay, 0);
//AudioConnection          patchSignal4(Delay, 0, Out, 0);
//AudioConnection          patchSignal5(Delay, 0, Out, 1);
//LoopBacks
AudioConnection          delayFeedback1(Delay, 0, delayMix1, 1);
AudioConnection          delayFeedback2(Delay, 1, delayMix1, 2);
AudioConnection          delayFeedback3(Delay, 2, delayMix1, 3);
AudioConnection          delayFeedback4(Delay, 3, delayMix2, 1);
AudioConnection          delayFeedback5(Delay, 4, delayMix2, 2);
AudioConnection          delayFeedback6(Delay, 5, delayMix2, 3);
AudioConnection          delayFeedback7(Delay, 6, delayMix3, 1);
AudioConnection          delayFeedback8(Delay, 7, delayMix3, 2);
AudioControlSGTL5000     audioShield;
//variables
float direct = 0.1;
float signalGain = 0.75;

//Declare encoders
Encoder enc1(0, 1);
long pos1 = 0; 
Encoder enc2(2, 3);
long pos2 = 0;
Encoder encFreq(4,5);
long posFreq = 0;
//number of rotations on control enc's
int rot = 4;

//Global Synthesis Variables
float freq = 55.0; //master frequency
float fade = 1; //cross fade between two waves

//Wave Synthsis Variables
float filterFreq = 0;
float q = 1;

//FM Synthesis Variables
float modIndex = 1;
float modAttn = 5;

//analog input things
int pStrip1Pin = A1; 
int pStrip2Pin = A2;
int pStrip3Pin = A3;
int ampPin = A7;
float pStrip1, pStrip2, pStrip3; 
float amp = 0.01; //master amplitude

void setup(void) {
  AudioMemory(128);
  audioShield.enable();
  audioShield.volume(0.6);
/*
  //Gains
  delayMix1.gain(0, signalGain);
  delayMix2.gain(0, signalGain);
  delayMix3.gain(0, signalGain);
  delayMix1.gain(1, direct);
  delayMix1.gain(2, direct);
  delayMix1.gain(3, direct);
  delayMix2.gain(1, direct);
  delayMix2.gain(2, direct);
  delayMix2.gain(3, direct);
  delayMix3.gain(1, direct);
  delayMix3.gain(2, direct);
  delayMix3.gain(3, 0);

  //set delay times
  for(int i = 0; i < 8; i++) {
    Delay.delay(i, (i*100)/4);
  }
}
*/
//Waveform Synthesis
Wave.begin(0.3, freq, WAVEFORM_TRIANGLE); //waveform type, etc.
Filt.frequency(filterFreq); //set filter frequency
Filt.octaveControl(1.5); //set how much the control signal changes the frequency 
Filt.resonance(q); //set Q

//FM Synthesis
Modulator.frequency(freq); //modulator frequency
Modulator.amplitude(0.3);  //modulator amplitude
FMattn.gain(0, modAttn); //modulator attenuation
Carrier.frequency(freq); //carrier frequency
Carrier.amplitude(0.4); //carrier amplitude
//CrossFade
Mix.gain(0, fade);
Mix.gain(1, abs(fade-1));
//Main envelope/amplitude control
EnvCtrl.amplitude(amp);
}


void loop() {
  //Do all the sensor readings
  pStrip1 = analogRead(pStrip1Pin); // 1 to 900
  pStrip2 = analogRead(pStrip2Pin); //100 to 900
  pStrip3 = analogRead(pStrip3Pin); //1 to 900
  amp = analogRead(ampPin); //1 to 900

  pos1 = enc1.read();
  pos2 = enc2.read();
  posFreq = encFreq.read();
  //Limit the encoder values
  if(pos1 < 0) enc1.write(0);
  if(pos1 > 96*rot) enc1.write(96*rot);
  if(pos2 < 0) enc2.write(0);
  if(pos2 > 96*rot) enc2.write(96*rot);
  if(posFreq < 0) encFreq.write(0);
  if(posFreq > 4096*10) encFreq.write(4096*10);

  filterFreq = linexp(pStrip2, 100, 900, 1.5, 10);
  amp = constrain(linexp(amp, 0, 900, 0.01, 1) - 0.02, 0, 1);
  freq = linexp(posFreq, 0, 4096*10, 40, 2000);
  fade = linlin(pos1, 0, 96*rot, 0.01, 1);
  modIndex = linlin(pos2, 0, (96*rot)*2, 1, 2);



  Wave.frequency(freq);
  Carrier.frequency(freq);
  Modulator.frequency(freq * modIndex);
  Filt.frequency(freq*filterFreq);
  Mix.gain(0, fade);
  Mix.gain(1, abs(fade-1));
  EnvCtrl.amplitude(amp, 1);
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

















