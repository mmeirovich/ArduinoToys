/*
 * MIDIMishka
 *
 *
 * Author: Mishka & Mishka
 * Use it wisely
 */ 

#include "MIDIUSB.h"


#define KNOB_MAX 1023
#define MIDI_MAX 127

typedef struct{
  byte pin;
  byte pinValue;
  byte control;
} pinToControl;

pinToControl pinToButtonMap[] = {{2, 0, 20}, {3, 0, 21}, {14, 0, 22}, {15, 0, 23}, {16, 0, 24}};
pinToControl pinToKnobMap[] = {{18, 0, 25}, {19, 0, 26}, {20, 0, 27}, {21, 0, 28}, {10, 0, 29}};

byte buttonsCount = 5;
byte knobsCount = 5;



void setup() {
  delay(5000);
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
  Serial.print("      sending button press"); 
    controlChange(0, control, 127*value);
    MidiUSB.flush();
}

// Sends given value to channel 0 of a given control
// Use it for sliders & potentiometers
void sendValueChange(byte control, byte value){
  Serial.print("send value change: control ");
  Serial.print(control);
  Serial.print(" value ");
  Serial.print(value);
    controlChange(0, control, value);    
    MidiUSB.flush();
}


void logPinWithInput(byte pin, int value){

    Serial.print("pin "); 
    Serial.print(pin); 
    Serial.print(" value "); 
    Serial.println(value);

}

byte knobToMidiKnob(int knobValue){
  double relation = KNOB_MAX / MIDI_MAX;
  return knobValue / relation;  
}

void loop() {
  // read buttons
  Serial.println("reading buttons"); 
  Serial.print(buttonsCount); 
  Serial.println(" buttons"); 

  for (int i=0; i<buttonsCount; i++){
    int pinValue = digitalRead(pinToButtonMap[i].pin);

    Serial.print("value read: "); 
    Serial.println(pinValue); 
    Serial.print(" value was: "); 
    Serial.println(pinToButtonMap[i].pinValue); 
    
    if (pinValue != pinToButtonMap[i].pinValue){
     
      Serial.print(" pin value changed"); 
      pinToButtonMap[i].pinValue = pinValue;
      sendButtonPress(pinToButtonMap[i].control, pinValue);  
                 
    }
    logPinWithInput(pinToButtonMap[i].pin, pinValue);
  }


  Serial.println("reading knobs"); 
  // read knobs
  for (int i=0; i<knobsCount; i++){
    
    int pinValue = analogRead(pinToKnobMap[i].pin);
    if (pinValue != pinToKnobMap[i].pinValue){
      pinToKnobMap[i].pinValue = pinValue;
  
      byte midiValue = knobToMidiKnob(pinValue);
      Serial.print(" from pin ");
      Serial.print(pinToKnobMap[i].pin);
      sendValueChange(pinToKnobMap[i].control, midiValue);
    }
    
    logPinWithInput(pinToKnobMap[i].pin, pinValue);
  }

    delay(1000);
}
