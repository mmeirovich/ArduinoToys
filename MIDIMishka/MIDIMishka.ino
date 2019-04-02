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

pinToControl pinToButtonMap[] = {{A0, 20}, {A1, 21}, {A2, 22}, {A3, 23}, {A4, 24}};
pinToControl pinToKnobMap[] = {{A5, 25}, {A6, 26}, {A7, 27}, {A8, 28}, {A9, 29}};

byte buttonsCount = sizeof(pinToButtonMap);
byte knobsCount = sizeof(pinToKnobMap);



void setup() {
  Serial.begin(115200);
  DDRD = B11111111; // set PORTD (digital 7~0) to outputs


  PORTD = B11110000;
}
// First parameter is the event type (0x09 = note on, 0x08 = note off).
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
void sendButtonPress(byte control){
    controlChange(0, control, 127);
    MidiUSB.flush();
}

// Sends given value to channel 0 of a given control
// Use it for sliders & potentiometers
void sendValueChange(byte control, byte value){
    controlChange(0, control, value);    
    MidiUSB.flush();
}



void readPin(int pinNumber){
  int pinValue = analogRead(pinNumber);
  Serial.print("just read pin ");
  Serial.print(pinNumber);
  Serial.print(" value ");
  Serial.println(pinValue);
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
  for (int i=0; i<buttonsCount; i++){
    int pinValue = digitalRead(pinToButtonMap[i].pin);
    if (pinValue == HIGH){
      if (pinToButtonMap[i].pinValue == LOW){
        sendButtonPress(pinToButtonMap[i].control);  
        pinToButtonMap[i].pinValue = HIGH;
      }      
    }else{
      pinToButtonMap[i].pinValue = LOW;
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
      sendValueChange(pinToKnobMap[i].control, pinValue);
    }
    
    logPinWithInput(pinToKnobMap[i].pin, pinValue);
  }

}
