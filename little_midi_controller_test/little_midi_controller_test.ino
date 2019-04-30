/*
Little midi controller buttons or sliders mux test
First mux controlled through D register
Pins:
arduino 2: B00000010 --> mux S0
arduino 3: B00000001 --> mux S1
arduino 4: B00010000 --> mux S2
arduino 6: B10000000 --> mux S3
*/

byte controlPins[] = {B00000000,  //read from Y0
                      B00000010,  //Y1
                      B00000001,  //Y2
                      B00000011,  //Y3
                      B00010000,  //Y4
                      B00010010,  //Y5
                      B00010001,  //Y6
                      B00010011,  //Y7
                      B10000000,  //Y8
                      B10000010,  //Y9
                      B10000001,  //Y10
                      B10000011,  //Y11
                      B10010000,  //Y12
                      B10010010,  //Y13
                      B10010001,  //Y14
                      B10010011}; //Y15

void setup() {
  Serial.begin(9600);
  DDRD = DDRD | B10010011; //control pins as output
  pinMode(9, OUTPUT); //led on pin 9
}

//set led brightness propotional to the input
void ledDisplay (int brightness) {
  brightness = brightness/4; 
  analogWrite(9, brightness);
}

void monitorData(int data, int muxInputPin) {
    Serial.print("Y");
    Serial.print(muxInputPin);
    Serial.print(":");
    Serial.print(data);
    Serial.print("  ");
    if (muxInputPin == 15) {
      Serial.println();
    }
}

void loop() {
  for (int i = 0; i < 16; i++)
  {
    PORTD = controlPins[i];   
    ledDisplay(analogRead(0));
    monitorData(analogRead(0), i);
    delay(300);
  }

}
