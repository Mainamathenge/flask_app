#include <SoftwareSerial.h>
SoftwareSerial gprsSerial(7,8);
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>    //https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library
LiquidCrystal_I2C lcd(0x27, 16, 2);
 
// TURBIDITY.

int Turbidity = A0;
float volt;
float ntu;

// TDS 

#include <EEPROM.h>
#include "GravityTDS.h"
float TDSd =0;
#define TdsSensorPin A2
float temperature = 25,tdsValue = 0;
GravityTDS gravityTds;

//PH

#define PHSensorPin A1          // the pH meter Analog output is connected with the Arduino’s Analog
unsigned long int avgValue;  //Store the average value of the sensor feedback
float b;
int buf[10],temp;
float phValued = 0;

void setup() {
// TUBIDITY DISPLAY
    Serial.begin(9600);
    lcd.begin();
    lcd.backlight();
// gsm setup
    gprsSerial.begin(9600); // the GPRS baud rate   
 
// TDS SETUP.
    pinMode(2,OUTPUT);
    gravityTds.setPin(TdsSensorPin);
    gravityTds.setAref(5.0);  //reference voltage on ADC, default 5.0V on Arduino UNO
    gravityTds.setAdcRange(1024);  //1024 for 10bit ADC;4096 for 12bit ADC
    gravityTds.begin();  //initialization
// PH SETUP.
    pinMode(3,OUTPUT);    
    Serial.println("Ready");    //Test the serial monitor
// DISPLAY
    lcd.begin();
    lcd.backlight();
    lcd.setCursor(1,0);
    lcd.print ("QWAS");
    lcd.setCursor(1,2);
    lcd.print("BOOTING..");
//solenoid relays
    pinMode(6,OUTPUT);
    pinMode(5,OUTPUT);
    
}

void loop() {
  lcd.clear();
  lcd.setCursor(0,5);
  lcd.print ("QWAS");
  lcd.setCursor(1,2);
  lcd.print("TESTING..");
  collect_sample ();
  delay (1000);
  test ();
  PH ();
  delay (1000);
  
  TDS ();
  delay (1000);
  TURBIDITY ();
  delay (1000);
  Send_data ();
  flash ();
  lcd.setCursor(1,2);
  lcd.print("TURBIDITY. ");
  lcd.print(ntu);
  lcd.setCursor(1,2);
  lcd.print("PH...  ");
  lcd.print(phValued);
  lcd.setCursor(1,2);
  lcd.print("TDS...  ");
  lcd.print(TDSd);
  

}
void PH ()
{
    Serial.print("got into ph");
  digitalWrite(3,HIGH);
  delay (5000);
  avgValue=analogRead(PHSensorPin);
  float phValue=(float)avgValue*5.0/1024; //convert the analog into millivolt
  phValue=3.5*phValue;                      //convert the millivolt into pH value
  Serial.print("    pH:");  
  Serial.println(phValue,2);
  phValued = phValue;
  digitalWrite(3, LOW); 
  Serial.println("out of ph");
}
void TDS ()
{
    digitalWrite(2, HIGH);
    delay (3000);
     Serial.println("got into TDS");
    //temperature = readTemperature();  //add your temperature sensor and read it
    gravityTds.setTemperature(temperature);  // set the temperature and execute temperature compensation
    gravityTds.update();  //sample and calculate
    tdsValue =  gravityTds.getTdsValue();  // then get the value
    Serial.print(tdsValue,0);
    Serial.println("ppm");
    delay(1000);
    digitalWrite(2,LOW);
    TDSd = tdsValue;
    Serial.print("LEFT TDS");
}
void TURBIDITY ()
{
    Serial.println("got to turbidity");
    delay(1500);
    volt = ((float)analogRead(Turbidity)/1023)*5;
    if(volt < 2.5)
    {
      ntu = 3000;
    }
    else
    {
      ntu = -1120.4*square(volt)+5742.3*volt-4353.8; 
    }
    
    Serial.print(volt);
    Serial.println(" V");
 
    lcd.setCursor(0,1);
    Serial.print(ntu);
    lcd.print(" NTU");
    delay(10);
    Serial.print("Leaving Turbidity");
}
void ShowSerialData()
{
  while(gprsSerial.available()!=0)
  Serial.write(gprsSerial.read());
  delay(5000); 
}
void Send_data ()
{  
  if (gprsSerial.available())
    Serial.write(gprsSerial.read());
 
  gprsSerial.println("AT");
  delay(1000);
 
  gprsSerial.println("AT+CPIN?");
  delay(1000);
 
  gprsSerial.println("AT+CREG?");
  delay(1000);
 
  gprsSerial.println("AT+CGATT?");
  delay(1000);
 
  gprsSerial.println("AT+CIPSHUT");
  delay(1000);
 
  gprsSerial.println("AT+CIPSTATUS");
  delay(2000);
 
  gprsSerial.println("AT+CIPMUX=0");
  delay(2000);
 
  ShowSerialData();
 
  gprsSerial.println("AT+CSTT=\" safaricom.com\"");//start task and setting the APN,
  delay(1000);
 
  ShowSerialData();
 
  gprsSerial.println("AT+CIICR");//bring up wireless connection   Safaricomgprs.com
  delay(3000);
 
  ShowSerialData();
 
  gprsSerial.println("AT+CIFSR");//get local IP adress
  delay(2000);
 
  ShowSerialData();
 
  gprsSerial.println("AT+CIPSPRT=0");
  delay(3000);
 
  ShowSerialData();
  
  gprsSerial.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"");//start up the connection
  delay(6000);
 
  ShowSerialData();
 
  gprsSerial.println("AT+CIPSEND");//begin send data to remote server
  delay(4000);
  ShowSerialData();
  
  String str="GET https://api.thingspeak.com/update?api_key=ZLB4PON9JBG3QQNF&field1=" + String(ntu) +"&field2="+String(tdsValue)+"&field3="+String(phValued);
  Serial.println(str);
  gprsSerial.println(str);//begin send data to remote server
  
  delay(4000);
  ShowSerialData();
 
  gprsSerial.println((char)26);//sending
  delay(5000);//waitting for reply, important! the time is base on the condition of internet 
  gprsSerial.println();
 
  ShowSerialData();
 
  gprsSerial.println("AT+CIPSHUT");//close the connection
  delay(100);
  ShowSerialData();
}
void collect_sample ()
{
  digitalWrite(6,HIGH);
  digitalWrite(5,LOW);
  delay(2000);
}
void test ()
{
  digitalWrite(6,LOW);
  digitalWrite(5,LOW);
  delay(2000);
}
void flash ()
{
  digitalWrite(6,HIGH);
  digitalWrite(5,HIGH);
  delay(2000);
}
