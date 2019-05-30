/*
 * MIDIMishka
 *
 *
 * Authors: Mishka & Mishka
 * Use it wisely
 */ 

#include "MIDIUSB.h"


#define KNOB_MAX 1023

#define PEDAL_MAX 991
#define MIDI_MAX 127

typedef struct{
  byte pin;
  int pinValue;
  byte control;
  byte midiValue;
} pinToControl;


pinToControl pinToButtonMap[] = {{5, 0, 20, 0}, {6, 0, 21, 0}, {7, 0, 22, 0}, {8, 0, 23, 0}, {9, 0, 24, 0}};
pinToControl pinToKnobMap[] = {{18, 0, 25, 0}, {19, 0, 26, 0}, {20, 0, 27, 0}, {21, 0, 28, 0}, {10, 0, 29, 0}};

pinToControl externalPedalMap = {A6, 0, 30, 0};

byte buttonsCount = 5;
byte knobsCount = 5;



void setup() {
  delay(1000);
  Serial.begin(9600);
}

// First parameter is the event type (0x09 = note on, 0x960008 = note off).
// Second parameter is note-on/note-off, combined with the channel.
// Channel can be anything between 0-15. Typically reported to the user as 1-16.
// Third parameter is the note number (48 = middle C).
// Fourth parameter is the velocity (64 = normal, 127 = fastest).

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

// First parameter is the event type (0x0B = control change).
// Second parameter is the event type, combined with the channel.
// Third parameter is the control number number (0-119).
// Fourth parameter is the control value (0-127).

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}


// Sends 127 to channel 0 to a given control.
// Use it for stateless buttons
void sendButtonPress(byte control, int value){
  //Serial.print("      sending button press"); 
    controlChange(0, control, 127*value);
    MidiUSB.flush();
}

// Sends given value to channel 0 of a given control
// Use it for sliders & potentiometers
void sendValueChange(byte control, byte value){
    Serial.print("send value change: control ");
    Serial.print(control);
    Serial.print(" value ");
    Serial.println(value);
    controlChange(0, control, value);    
    MidiUSB.flush();
}


void logPinWithInput(byte pin, int value){

    Serial.print("pin "); 
    Serial.print(pin); 
    Serial.print(" value "); 
    Serial.println(value);

}


byte expressionPedalToMidiKnob(int knobValue){
  
  static double relation = (double)PEDAL_MAX / (double)MIDI_MAX;

  Serial.print(" source pin value ");
  Serial.println(knobValue);     
  Serial.print(" relation ");
  Serial.println(relation);     
  Serial.print(" in brackets ");
  Serial.println(PEDAL_MAX - knobValue);     

  Serial.print(" sending back ");
  Serial.println((PEDAL_MAX - knobValue) / relation);     

  // our expression pedal when down shows MIDI_MAX and when up shows 0. That's just how it is.
  return round(double(PEDAL_MAX - knobValue) / relation);  
}



byte knobToMidiKnob(int knobValue){
  double relation = KNOB_MAX / MIDI_MAX;
  return (KNOB_MAX - knobValue) / relation;  
}


// return true if two values have a serious gap between them, in our case 2 or more
// solves the problem of false knob values changes that we discovered while experimenting
bool valueSignificantlyDifferent(int value1, int value2){

  if (value1 != value2){
    return abs(value1-value2) > 3;
  }
  return false;
}


void loop() {
  
  // read buttons
//  Serial.println("");
//  Serial.println("reading buttons"); 

  for (int i=0; i<buttonsCount; i++){
    int pinValue = digitalRead(pinToButtonMap[i].pin);
    
    if (pinValue != pinToButtonMap[i].pinValue){
     
      Serial.println(" button value changed"); 
      pinToButtonMap[i].pinValue = pinValue;

      logPinWithInput(pinToButtonMap[i].pin, pinValue);
      sendButtonPress(pinToButtonMap[i].control, pinValue);  
      delay(50); // to try to avoid current leakage to the analog pins
    }
    
  }

//  Serial.println("");
//  Serial.println("reading knobs"); 
  // read knobs
  for (int i=0; i<knobsCount; i++){
    
    int pinValue = analogRead(pinToKnobMap[i].pin);
    if (valueSignificantlyDifferent(pinValue, pinToKnobMap[i].pinValue)){
      Serial.println("KNOB");
      Serial.print(" knob value changed. from pin ");
      Serial.println(pinToKnobMap[i].pin);
      Serial.print(" previous value ");
      Serial.println(pinToKnobMap[i].pinValue);      
      Serial.print(" new value ");
      Serial.println(pinValue);      
      Serial.println("END KNOB");
      
      pinToKnobMap[i].pinValue = pinValue;
  
      byte midiValue = knobToMidiKnob(pinValue);
      if (midiValue != pinToKnobMap[i].midiValue){
        logPinWithInput(pinToKnobMap[i].pin, pinValue);
        
        sendValueChange(pinToKnobMap[i].control, midiValue);
        pinToKnobMap[i].midiValue = midiValue;
      }
      
      
    }
    
    
  }

//  Serial.println("");
//  Serial.println("reading external expression pedal");


  int pinValue = analogRead(externalPedalMap.pin);
  if (valueSignificantlyDifferent(pinValue, externalPedalMap.pinValue )){

    externalPedalMap.pinValue = pinValue;

    byte midiValue = expressionPedalToMidiKnob(pinValue);

    Serial.print(" expression pedal value ");
    Serial.println(pinValue);     
    if (midiValue != externalPedalMap.midiValue){
      externalPedalMap.midiValue = midiValue;
      sendValueChange(externalPedalMap.control, midiValue);
    }
    
    
  }   

  
  //delay(1000);
}
