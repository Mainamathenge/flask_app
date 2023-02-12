#include <SPI.h>
#include <Wire.h>

#include <WEMOS_SHT3X.h>

 //Wire.begin();

SHT3X sht30(0x45);

float Humidity = 0.00 ;
float temperature = 0.00 ;

unsigned long previousMillis = 0; 
const long interval = 2000;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

}

void getTemperature() {
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    sht30.get();
    Humidity = (sht30.humidity);  
    temperature = (sht30.cTemp);
    Serial.print("temp");
    Serial.println(temperature);
    }
}

void loop() {
  // put your main code here, to run repeatedly:
 getTemperature();

}
