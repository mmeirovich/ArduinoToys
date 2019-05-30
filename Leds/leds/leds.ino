int led1 = 13; // the pin the LED is connected to
int led2 = 19;  // analog
int potPin = 2;
int potValue = 0;

int redLedPins[] = {11,12, 13};
int greenLedPins[] = {8, 9, 10};

int delayTime = 500;

void setup() {
//  pinMode(led1, OUTPUT); // Declare the LED as an output
//  pinMode(led2, OUTPUT);

  for (int i=0; i<3; i++){
    pinMode(redLedPins[i], OUTPUT);
    pinMode(greenLedPins[i], OUTPUT);
  }
  delayTime = analogRead(potPin) + 1;
  Serial.begin(9600);
}

void doSteps(int pins[], byte size){
  for (int i=0; i<size; i++){
    digitalWrite(pins[i], HIGH);
    delayTime = analogRead(potPin) + 1;
    delay(delayTime);
    digitalWrite(pins[i], LOW);    
  }
}


void loop() {
  doSteps(redLedPins, 3);
  doSteps(greenLedPins, 3);
}
