typedef struct{
  byte pin;
  byte pinValue;
  byte control;
} pinToControl;

pinToControl pinToButtonMap[] = {{5, 0, 20}, {6, 0, 21}, {7, 0, 22}, {8, 0, 23}, {9, 0, 24}};
pinToControl pinToKnobMap[] = {{18, 0, 25}, {19, 0, 26}, {20, 0, 27}, {21, 0, 28}, {10, 0, 29}, {A6, 0, 30}};

byte knobsCount = 6;
byte buttonsCount = 5;

void setup() {
  Serial.begin(9600);
}

void logPinWithInput(byte pin, int value){

    Serial.print("pin "); 
    Serial.print(pin); 
    Serial.print(" value "); 
    Serial.println(value);

}

void loop() {
  Serial.println("reading buttons");
  // read buttons
  for (int i=0; i<buttonsCount; i++){
  int pinValue = digitalRead(pinToButtonMap[i].pin);
  logPinWithInput(pinToButtonMap[i].pin, pinValue); 
  }

  Serial.println("reading knobs"); 
  // read knobs
  for (int i=0; i<knobsCount; i++){   
    int pinValue = analogRead(pinToKnobMap[i].pin);
    logPinWithInput(pinToKnobMap[i].pin, pinValue);

   }
  
  delay(2000);
}
