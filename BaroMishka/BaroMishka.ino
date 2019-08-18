/*
 * Barometer
 * by Mishka & Mishka
 * 
 */

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#include <TM1637.h>

#define CLK 5
#define DIO 6

const float PRESSURE_CALIBRATION = 1013.25;

TM1637 display1(CLK, DIO);

// using I2C
Adafruit_BMP280 bme;

enum screen_state {
  TEMPERATURE = 1,
  PRESSURE = 2,
  ALTITUDE = 3
};

screen_state state = ALTITUDE;

const int buttonPin = 2;

void ShowDigits(int8_t digit1, int8_t digit2, int8_t digit3, int8_t digit4){
  
  int8_t digits[] = {0,1,2,3};
  
  digits[0] = digit1 ;
  digits[1] = digit2 ;
  digits[2] = digit3 ;
  digits[3] = digit4 ;

  display1.display(digits);
}


void ShowNumber(int number){
  int8_t digits[4];  
  Serial.print(number / 1000);
  Serial.print(number % 1000 / 100);
  Serial.print(number % 1000 % 100 / 10);
  Serial.print(number % 1000 % 100 % 10);
  
  
  digits[0] = number / 1000;
  
  digits[1] = number % 1000 / 100;
  digits[2] = number % 1000 % 100 / 10;
  digits[3] = number % 1000 % 100 % 10;

  display1.display(digits);
}

void NextState(){
  switch(state){
    case TEMPERATURE:
      state = PRESSURE;
      break;
    case PRESSURE:
      state = ALTITUDE;
      break;
    case ALTITUDE:
      state = TEMPERATURE;
      break;
  }
}

bool ButtonPressed(){
  return (digitalRead(buttonPin) == HIGH);
}
  
void setup()
{  
    
  Serial.begin(9600);
  if (!bme.begin(0x76, 0x58))
  {  
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    ShowNumber(9999);
    while (1);
  }

  display1.set();
  display1.init();
}


 
void loop()
{

  
    Serial.print("---- GY BMP 280 ----------------\n");
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" *C");
    Serial.print("Pressure = ");
    Serial.print(bme.readPressure() / 100); // 100 Pa = 1 millibar
    Serial.println(" mb");
    Serial.print("Approx altitude = ");
    Serial.print(bme.readAltitude(PRESSURE_CALIBRATION));   
    Serial.println(" m");
    Serial.print("--------------------------------\n\n");

    int toDisplay = 0;

    switch(state){
      case TEMPERATURE:
        toDisplay = bme.readTemperature();
        break;
      case PRESSURE:
        toDisplay = bme.readPressure();
        break;
      case ALTITUDE:
        toDisplay = bme.readAltitude(PRESSURE_CALIBRATION);
        break;
    }
    ShowNumber(toDisplay);
    delay(5000);
    NextState();
}
