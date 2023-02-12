#include <SPI.h>

#include <Servo.h>
Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

/* Include the RFID library */
#include <RFID.h>

/* Define the DIO used for the SDA (SS) and RST (reset) pins. */
#define SDA_DIO 9
#define RESET_DIO 8
int productname[5]={228,18,37,75,24};
int product[5]={100,120,230,125,70};
int token[5]={0,0,0,0,0};
int Total;
/* Create an instance of the RFID library */
RFID RC522(SDA_DIO, RESET_DIO); 

void setup() {
    Serial.begin(9600);
  /* Enable the SPI interface */
  SPI.begin(); 
  /* Initialise the RFID reader */
  RC522.init();
 myservo.attach(2); 
  // put your setup code here, to run once:

}

void loop() {
  if (RC522.isCard())
  {
    /* If so then get its serial number */
    RC522.readCardSerial();
    Serial.print("card read");
  }
  // put your main code here, to run repeatedly:

}
