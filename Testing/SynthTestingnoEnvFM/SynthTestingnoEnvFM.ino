#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

//#define ENCODER_DO_NOT_USE_INTERRUPTS
#include<Encoder.h>


Encoder knobLeft(0, 1);
Encoder knobRight(2, 3);
Encoder pitchEnc(15,16);

long positionLeft = 0;
long positionRight = 0;
long positionPitch = 0;
float cutOff = 800;
float freq = 220;
float freqFM = 500;
float mixGain0 = 0.1;

// GUItool: begin automatically generated code
AudioSynthWaveform       waveform1;      //xy=97,156
AudioEffectEnvelope      envelope1;      //xy=237,157
AudioSynthWaveformSineModulated sine_fm1;       //xy=293,214
AudioFilterBiquad        biquad1;        //xy=402,158
AudioMixer4              mixer1;         //xy=548,175
AudioOutputI2S           i2s1;           //xy=687,176
AudioConnection          patchCord1(waveform1, envelope1);
AudioConnection          patchCord2(waveform1, sine_fm1);
AudioConnection          patchCord3(envelope1, biquad1);
AudioConnection          patchCord4(sine_fm1, 0, mixer1, 1);
AudioConnection          patchCord5(biquad1, 0, mixer1, 0);
AudioConnection          patchCord6(mixer1, 0, i2s1, 0);
AudioConnection          patchCord7(mixer1, 0, i2s1, 1);
// GUItool: end automatically generated code
AudioControlSGTL5000     audioShield;
// GUItool: end automatically generated code

void setup(void) {
  Serial.begin(115200);
  Serial.println("begin");
  // Set up
  AudioMemory(128);
  audioShield.enable();
  audioShield.volume(0.5); 
  waveform1.begin(0.4, freq, WAVEFORM_TRIANGLE);
  biquad1.setLowpass(0, cutOff, 0.7);
  mixer1.gain(0, mixGain0);
  mixer1.gain(1, 0.4);
  sine_fm1.frequency(freqFM);
  biquad1.setLowpass(0, 800, 0.7);
}

float val;
void loop() {
  val = analogRead(3);
  val = map(val, 600, 900, 0, 1000);
  val = val/1000;
  Serial.println(val);
  delay(10);
  long newLeft, newRight, newPitch;
  newLeft = knobLeft.read();
  newRight = knobRight.read();
  newPitch = pitchEnc.read();

  if (newLeft != positionLeft || newRight != positionRight || newPitch != positionPitch) {
    /*
     Serial.print("Left = ");
     Serial.print(newLeft);
     Serial.print(", Right = ");
     Serial.print(newRight);
     Serial.print(", Pitch = ");
     Serial.print(newPitch);
     Serial.println();
     */
    positionLeft = newLeft;
    positionRight = newRight;
    positionPitch = newPitch;
    Serial.println(freq);
    Serial.println(positionLeft/100.0);
    freqFM = linexp(newPitch, 0, 50000, 40, 110*8);
    freq = linexp(newPitch, 0, 50000, 1, 110*10);

    AudioNoInterrupts();

    waveform1.frequency(freq);
    sine_fm1.frequency(freqFM);
    AudioInterrupts();
  }
  AudioNoInterrupts();
  mixer1.gain(0, val);
  mixer1.gain(1, val);
  AudioInterrupts();
}

float linexp (float x, float a, float b, float c, float d)
//intput, input low, input high, output low, output high
{
  if (x <= a) return c;
  if (x >= b) return d;
  return pow(d/c, (x-a)/(b-a)) * c;
}













