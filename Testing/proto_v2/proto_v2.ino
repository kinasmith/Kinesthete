//INIT Libraries
#include <Encoder.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

//Create UGen's
AudioSynthWaveformSine   Modulator;
AudioSynthWaveform       Wave;

AudioSynthWaveformSine       Partial_1;
AudioSynthWaveformSine       Partial_2;

AudioMixer4              FMattn;
AudioFilterStateVariable Filt;
AudioSynthWaveformSineModulated Carrier;
AudioMixer4              Mix;
AudioSynthWaveformDc     EnvCtrl;
AudioEffectMultiply      VCA;
AudioOutputI2S           Out; 
//Make patch Connections
AudioConnection          patchCord1(Modulator, 0, FMattn, 0);
AudioConnection          patchCord2(FMattn, 0, Carrier, 0);
AudioConnection          patchCord3(Carrier, 0, Mix, 0);
AudioConnection          patchCord4(Wave, 0, Filt, 0);
AudioConnection          patchCord5(Filt, 0, Mix, 1);
AudioConnection          patchCord6(EnvCtrl, 0, VCA, 1);
AudioConnection          patchCord7(Mix, 0, VCA, 0);
AudioConnection          patchCord8(Partial_1, 0, Mix, 2);
AudioConnection          patchCord9(Partial_2, 0, Mix, 3);



AudioConnection          patchCord20(VCA, 0, Out, 0);
AudioConnection          patchCord21(VCA, 0, Out, 1);
AudioControlSGTL5000     audioShield;

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
float freqMIDI = 20;
float fade = 1; //cross fade between two waves

//Wave Synthsis Variables
float filterFreq = 0;
float q = 1;
float partial_1amp;
float partial_2amp;


//FM Synthesis Variables
float modIndex = 1;
float modAttn = 5;

//analog input things
int pStrip1Pin = A3; 
int pStrip2Pin = A2;
int pStrip3Pin = A1;
int ampPin = A6;
float pStrip1, pStrip2, pStrip3; 
float amp = 0.01; //master amplitude

void setup(void) {
  pinMode(21, OUTPUT);

  AudioMemory(8);
  audioShield.enable();
  audioShield.volume(0.6);
  //Waveform Synthesis
  Wave.begin(0.3, freq, WAVEFORM_TRIANGLE); //waveform type, etc.
  Partial_1.amplitude(0.2);
    Partial_2.amplitude(0.2);
  //Partial_1.begin(0.3, freq, WAVEFORM_TRIANGLE); //waveform type, etc.
  //Partial_1.begin(0.3, freq, WAVEFORM_TRIANGLE); //waveform type, etc.

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
  analogWrite(21, int(linexp(amp, 0, 900, 255, 0.1))-5);

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
  partial_1amp = linlin(pStrip1, 0, 700, 0.01, 0.9);
  partial_2amp = linlin(pStrip3, 0, 700, 0.01, 0.9);
  amp = constrain((linexp(amp, 0.1, 500, 0.1, 1)-0.1), 0, 1);
  //freq = linexp(posFreq, 0, 4096*10, 20, 127);
  freqMIDI = linlin(posFreq, 0, 4096*10, 36, 100);
  fade = linlin(pos1, 0, 96*rot, 0.01, 1);
  modIndex = linlin(pos2, 0, (96*rot)*2, 1, 2);
  freq = midicps(freqMIDI);
  
  Wave.frequency(freq);
  Partial_1.frequency(midicps(freqMIDI+3));
  Partial_2.frequency(midicps(freqMIDI+7));
  Partial_1.amplitude(partial_1amp-0.5);
  Partial_2.amplitude(partial_2amp-0.5);

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

/*-------------------------------------------------------------------
 * midicps: Map from MIDI note to frequency
 *-------------------------------------------------------------------*/
float midicps (float note)
{
  return 440.0 * pow(2, (note - 69) / 12);
}

/*-------------------------------------------------------------------
 * cpsmidi: Map from frequency to MIDI note
 *-------------------------------------------------------------------*/
float cpsmidi (float freq)
{
  return (log(freq / 440.0) / log(2.0)) * 12 + 69;
}

























