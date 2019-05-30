
/*
 * MIDIMishkaB
 * Our second MIDI controller
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
} pinToControl;


pinToControl muxPinToKnobMap[] = { {B00000000, 0, 20}, 
                                  {B00000010, 0, 21}, 
                                  {B00000001, 0, 22}, 
                                  {B00000011, 0, 23}, 
                                  {B00010000, 0, 24}, 
                                  {B00010010, 0, 25}, 
                                  {B00010001, 0, 26}, 
                                  {B00010011, 0, 27}, 
                                  {B10000000, 0, 28}, 
                                  {B10000010, 0, 29}, 
                                  {B10000001, 0, 30}, 
                                  {B10000011, 0, 31}, 
                                  {B10010000, 0, 32}, 
                                  {B10010010, 0, 33}};


pinToControl muxPinToButtonMap[] = {{B10010001, 0, 35}, 
                                    {B10010011, 0, 36}};


pinToControl boardPinToButtonMap[] = {{5, 0, 43}, 
                                      {7, 0, 39}, 
                                      {8, 0, 38}, 
                                      {9, 0, 37}, 
                                      {10, 0, 40}, 
                                      {16, 0, 41}, 
                                      {14, 0, 42}, 
                                      {15, 0, 44}};



byte boardButtonsCount = 8;
byte muxButtonsCount = 2;
byte knobsCount = 14;

void setup() {
  delay(1000);
  Serial.begin(9600);

  DDRD = DDRD | B10010011; //set control pins as output
  PORTD = PORTD & B01101100; // set only the 4 pins to zero w/o touching any others.   

  for (int i=0; i<boardButtonsCount; i++){
    pinMode(boardPinToButtonMap[i].pin, INPUT_PULLUP);
  }
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



byte knobToMidiKnob(int knobValue){
  double relation = KNOB_MAX / MIDI_MAX;
  return MIDI_MAX - (KNOB_MAX - knobValue) / relation;  
}

byte buttonToMidi(byte buttonValue){
  return buttonValue < 200 ? 127 : 0;
}
// return true if two values have a serious gap between them, in our case 2 or more
// solves the problem of false knob values changes that we discovered while experimenting
bool valueSignificantlyDifferent(int value1, int value2){

  if (value1 != value2){
    return abs(value1-value2) > 3;
  }
  return false;
}

int readMuxPin(byte pinNumber){
    PORTD = pinNumber;
    int pinValue = analogRead(0);  
    return pinValue;
}

byte digitalReadMuxPin(byte pinNumber){
    PORTD = pinNumber;
    byte pinValue = digitalRead(0);  
    return pinValue;
}



void loop() {


  // read knobs via mux pins  
  for (int i = 0; i < knobsCount; i++)
  {    
    int pinValue = readMuxPin(muxPinToKnobMap[i].pin);
    if (valueSignificantlyDifferent(pinValue, muxPinToKnobMap[i].pinValue)){
        muxPinToKnobMap[i].pinValue = pinValue;

        Serial.println("mux knobs"); 

        byte midiValue = knobToMidiKnob(pinValue);
        logPinWithInput(muxPinToKnobMap[i].pin, pinValue);
        
        sendValueChange(muxPinToKnobMap[i].control, midiValue);
    }
    //monitorData(analogRead(0), i);
    //delay(300);
  }

  // read buttons via mux pins
  for (int i = 0; i < muxButtonsCount; i++)
  {    
    byte pinValue = readMuxPin(muxPinToButtonMap[i].pin);
    byte midiValue = buttonToMidi(pinValue);
    // since we use analog here for digital signal, it's better to store the digital result rather than the analog value
    if (midiValue != muxPinToButtonMap[i].pinValue){        

      
        muxPinToButtonMap[i].pinValue = midiValue;

        
        logPinWithInput(muxPinToButtonMap[i].pin, pinValue);
        
        sendValueChange(muxPinToButtonMap[i].control, midiValue);
    }
  }


  for (int i=0; i<boardButtonsCount; i++){
    int pinValue = digitalRead(boardPinToButtonMap[i].pin);
    
    if (pinValue != boardPinToButtonMap[i].pinValue){
     
      Serial.println("board button"); 
      boardPinToButtonMap[i].pinValue = pinValue;
      
      byte midiValue = pinValue == 0 ? 127 : 0;
      logPinWithInput(boardPinToButtonMap[i].pin, pinValue);
      sendValueChange(boardPinToButtonMap[i].control, midiValue);  
      //delay(50); // to try to avoid current leakage to the analog pins
    }
    
  }


  
  //delay(1000);
}
