#include <SPI.h>

#include <Servo.h>
#include <RFID.h>
#include <Servo.h>
Servo myservo;  // create servo object to control a servo

/* Define the DIO used for the SDA (SS) and RST (reset) pins. */
#define SDA_DIO 9
#define RESET_DIO 8
int productname[5]={228,18,37,75,24};
int product[5]={100,120,230,125,70};
int token[5]={0,0,0,0,0};
int Total;
/* Create an instance of the RFID library */
RFID RC522(SDA_DIO, RESET_DIO); 
  byte i=0;
  byte j=0;
  byte k=0;
  int ID;
void setup()
{ 
  Serial.begin(9600);
  /* Enable the SPI interface */
  SPI.begin(); 
  /* Initialise the RFID reader */
  RC522.init();
  
  RC522.init();
 myservo.attach(2);
  
}

void loop()
{
  /* Temporary loop counter */


  /* Has a card been detected? */
  //for (i=0;i<10;i++)
  //{
  if (RC522.isCard())
  {
    /* If so then get its serial number */
    RC522.readCardSerial();
    Serial.print(RC522.serNum[i],DEC);

    Serial.println("Card detected:");
    myservo.write(90);
    delay(100);
    myservo.write(0);

    /* Output the serial number to the UART */
    ID=RC522.serNum[0];
    //Serial.print(ID);
    Serial.println(" ");
    for(i=0;i<15;i++)
    {
      /*if(productname[i]==ID)
      {
        Serial.println("Total Purchase");
        if(token[i]==0)
        {
          Total=Total+product[i];
          token[i]=1;
        }
        else
        {
          Total=Total-product[i];
          token[i]=0;
        }
        */
        Serial.println("access granted");
        i++;
        delay(1000);
        
       if(i==5)
      { 
        Serial.println("Access Denied"); 
        //break;
      }
      break;
      }
      
  }
     

 
}
