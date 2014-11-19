//INIT Libraries
#include <Bounce.h>
#include <Encoder.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

//Initialize Audio Library Objects
//AudioSynthWaveformSine   modulator1;
AudioSynthWaveform   modulator1;
AudioEffectMultiply      vcaModulator;
AudioSynthWaveformDc     dcModulator;
AudioSynthWaveformSineModulated carrier1;  
AudioSynthWaveform       modSaw;
AudioSynthWaveformDc     dc1;
AudioEffectMultiply      vca1;

AudioSynthWaveformSine   modulator2;
AudioSynthWaveformSineModulated carrier2; 
AudioSynthWaveformDc     dc2;
AudioEffectMultiply      vca2;

AudioMixer4              mixer;
AudioOutputI2S           out;
AudioControlSGTL5000     audioShield;

//Set Audio Routes
AudioConnection          osc1p0(modulator1, vcaModulator);
AudioConnection          osc1p1(vcaModulator, 0, carrier1, 0);
AudioConnection          osc1p2(dcModulator, 0, vcaModulator, 1);
AudioConnection          osc1p3(carrier1, 0, vca1, 0);
AudioConnection          osc1p4(dc1, 0, vca1, 1);
AudioConnection          osc1p5(vca1, 0, mixer, 0);
AudioConnection          osc2p1(modulator2, vca2);
AudioConnection          osc2p2(dc2, 0, vca2, 1);
AudioConnection          osc2p3(vca2, 0, mixer, 1);
AudioConnection          outp1(mixer, 0, out, 0);
AudioConnection          outp2(mixer, 0, out, 1);

//Declare encoders and position holder variables
Encoder enc0(28, 29);
long pos0 = 0; 
long oldPos0 = 0;
Encoder enc1(27, 30);
long pos1 = 0;
long oldPos1 = 0;
Encoder enc2(26, 31);
long pos2 = 0;
long oldPos2 = 0;
Encoder enc3(7, 6);
long pos3 = 0;
long oldPos3 = 0;
Encoder encFreq(25,24);
long posFreq = 0;
long oldPosFreq = 0;
int rot = 4; //number of rotations on control encoders

//Declare Switch Pins and De-Bouncing
int switch1Pin = 5;
int switch2Pin = 4;
int switch3Pin = 3;
int switch4Pin = 2;
Bounce switch1 = Bounce(switch1Pin, 5); 
Bounce switch2 = Bounce(switch2Pin, 5); 
Bounce switch3 = Bounce(switch3Pin, 5); 
Bounce switch4 = Bounce(switch4Pin, 5); 

//Global Synthesis Variables

//---------- Oscillator 1 ----------------//
float osc1Freq = 110.0; //Master OSC frequency
float osc1FreqMIDI = 20; //Master OSC MIDI
float modulatorAmp; //Modulator Amplitude (controlled by pStrip2
float modFreq; //Modulator Frequency (a function of osc1Freq)

//---------- Oscillator 2 ----------------//
float osc2Freq = 110.0; //Osc2 Frequency
float osc2FreqMIDI = 20; //Osc2 MIDI
float osc2Offset = 0; //offset between OSC1 and OSC2 when in FollowMode
int followPitch = 1; //boolean for pitch Following
int followState = 0; //buttonPress state
int lastFollowState = 0; 

//---------- Control Inputs ----------------//
int amp1Pin = A13; //amplitude for OSC1
float amp1Read = 0;
float amp1 = 0; 
float oldAmp1 = 0;
int amp2Pin = A12; //amplitude for OSC2
float amp2Read = 0;
float amp2 = 0; 
float oldAmp2 = 0;
//int pStrip1Pin = ; //
int pStrip2Pin = A7; //main pressure strip Pin
float pStrip1;
float oldpStrip1 = 0;
float pStrip2; //Pressure Strips
float oldpStrip2 = 0;

//---------- LED Control ----------------//
char colors[] = {
  'r', 'g', 'b'};
int led0Val;
int led1Val;
int led2Val;
int led3Val;



void setup(void) {
  Serial1.begin(9600);
  //write all LED's to RED to indicate pitch needs to be set
  ledSet(0, 'r', 4095);

  //Setup Switch Pin Modes
  pinMode(switch1Pin, INPUT);
  pinMode(switch2Pin, INPUT);
  pinMode(switch3Pin, INPUT);

  AudioMemory(120);
  audioShield.enable();
  audioShield.volume(0.8);
modulator1.begin(1, 0, WAVEFORM_SAWTOOTH);
//  modulator1.amplitude(1);
  modulator2.amplitude(0.3);

  mixer.gain(0, 0.8);
  mixer.gain(1, 0.8);

  enc0.write(40);
  enc2.write(96*rot);
  if(followPitch){
    ledSet(2, 'r', 1500); 
    ledSet(2, 'b', 0);
  }
  else {
    ledSet(2, 'r', 0); 
    ledSet(2, 'b', 1000);
  }

}

void loop() {
  switch1.update();
  switch2.update();
  switch3.update();
  switch4.update();

  osc2FollowState();

  if(switch1.read()) {
    encFreq.write(0); 
    //write all LED's LOW to indicate Pitch is Set
    ledSet(0, 'r', 0);
  } 
  /*
  pStrip2 = analogRead(pStrip2Pin); 
   if(pStrip2 != oldpStrip2) {
   oldpStrip2 = pStrip2;
   }
   */
  pStrip2 = analogRead(pStrip2Pin); 
  if(pStrip2 != oldpStrip2) {
    oldpStrip2 = pStrip2;
    modulatorAmp = linexp(pStrip2, 100, 900, 0.2, 1);
  }
  amp1Read = analogRead(amp1Pin);
  if(amp1Read != oldAmp1) {
    oldAmp1 = amp1Read;
    amp1 = constrain((linexp(amp1Read, 40.0, 700, 0.1, 1)-0.1), 0, 1);
    // ledSet(1, 'b', map(amp1Read, 0, 900, 0, 4095));
  }

  amp2Read = analogRead(amp2Pin);
  if(amp2Read != oldAmp2) {
    oldAmp2 = amp2Read;
    amp2 = constrain((linexp(amp2Read, 40.0, 700, 0.1, 1)-0.1), 0, 1);
    // ledSet(1, 'g', map(amp2Read, 0, 900, 0, 4095));
  }

  pos0 = enc0.read();
  if(pos0 < 0) enc0.write(0);
  if(pos0 > 96*rot) enc0.write(96*rot);
  if(pos0 != oldPos0) {
    oldPos0 = pos0;
    led0Val = map(pos0, 0, 96*rot, 0, 4095);  
    ledSet(0, 'g', led0Val);
  }

  pos1 = enc1.read();
  if(pos1 < 0) enc1.write(0);
  if(pos1 > 96*rot) enc1.write(96*rot);
  if(pos1 != oldPos1) {
    oldPos1 = pos1;
    led1Val = map(pos1, 0, 96*rot, 0, 4095);  
    ledSet(1, 'g', led1Val);
  }

  pos2 = enc2.read();
  if(pos2 < 0) enc2.write(0);
  if(pos2 > 96*rot) enc2.write(96*rot);
  if(pos2 != oldPos2) {
    oldPos2 = pos2;
    led2Val = map(pos2, 0, 96*rot, 0, 4095);  
    ledSet(2, 'g', led2Val);
  }
  pos3 = enc3.read();
  if(pos3 < 0) enc3.write(0);
  if(pos3 > 96*rot) enc3.write(96*rot);
  if(pos3 != oldPos3) {
    oldPos3 = pos3;
    led3Val = map(pos3, 0, 96*rot, 0, 4095);  
    ledSet(3, 'g', led3Val);
  }
  posFreq = encFreq.read();

  osc1FreqMIDI = linlin(posFreq, 0, 4096*12, 36, 100);
  osc1Freq = midicps(osc1FreqMIDI);
  if(followPitch) {
    osc2Offset = linlin(pos2, 0, 96*rot, 0, 24) - 12;
    osc2FreqMIDI = osc1FreqMIDI+osc2Offset;
    osc2Freq = midicps(osc2FreqMIDI);
  } 
  else {
    osc2FreqMIDI = linlin(pos2, 0, 96*rot, 20, 100);
    osc2Freq = midicps(osc2FreqMIDI);
  }

  //---------- Synthesis!!! ----------------//

  modFreq = linlin(pos0, 0, 96*rot, 0, 2);

  carrier1.frequency(osc1Freq);
  modulator1.frequency((osc1Freq*modFreq));
  dcModulator.amplitude(amp1*modulatorAmp, 1);
  dc1.amplitude(amp1, 1);

  carrier2.frequency(osc2Freq);
  modulator2.frequency(osc2Freq);
  dc2.amplitude(amp2, 1);

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

void ledSetAll(int val) {
  val = abs(val - 4095);
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j < sizeof colors; j++) {
      Serial1.print(i);
      Serial1.print(" ");
      Serial1.print(colors[j]);
      Serial1.print(" ");
      Serial1.print(val);
      Serial1.print(".");
    } 
  }
}

void ledSetAllColor(char col, int val){
  val = abs(val - 4095);
  for(int i = 0; i < 4; i++) {
    Serial1.print(i);
    Serial1.print(" ");
    Serial1.print(col);
    Serial1.print(" ");
    Serial1.print(val);
    Serial1.print(".");
  }
}

void ledSet(int enc, char col, int val) {
  val = abs(val - 4095);
  Serial1.print(enc);
  Serial1.print(" ");
  Serial1.print(col);
  Serial1.print(" ");
  Serial1.print(val);
  Serial1.print(".");
}


void osc2FollowState() {
  followState = switch3.read();

  if(followState != lastFollowState) {
    if(followState == HIGH){
      followPitch = !followPitch;
    }
    if(followPitch){
      ledSet(2, 'r', 1500); 
      ledSet(2, 'b', 0);
    }
    else {
      ledSet(2, 'r', 0); 
      ledSet(2, 'b', 1000);
    }
  }
  lastFollowState = followState;
}













