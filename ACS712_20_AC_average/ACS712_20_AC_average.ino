//
//    FILE: ACS712_20_AC_average.ino
//  AUTHOR: Rob Tillaart
// PURPOSE: demo AC measurement with point to point + averaging
//     URL: https://github.com/RobTillaart/ACS712


#include "ACS712.h"


//  Arduino UNO has 5.0 volt with a max ADC value of 1023 steps
//  ACS712 5A  uses 185 mV per A
//  ACS712 20A uses 100 mV per A
//  ACS712 30A uses  66 mV per A


ACS712  ACS(A0, 5.0, 1023, 100);
//  ESP 32 example (might requires resistors to step down the logic voltage)
//  ACS712  ACS(25, 3.3, 4095, 185);
int ledPin = 3;

void setup()
{
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  while (!Serial);
  Serial.println(__FILE__);
  Serial.print("ACS712_LIB_VERSION: ");
  Serial.println(ACS712_LIB_VERSION);

  ACS.autoMidPoint();
  Serial.print("MidPoint: ");
  Serial.println(ACS.getMidPoint());


  Serial.print("Noise mV: ");
  Serial.println(ACS.getNoisemV());
}
void loop()
{
  float scale = 0;
  float norm = 0 ;
  float average = 0;
  uint32_t start = millis();
  for (int i = 0; i < 100; i++)
  {
    //  select sppropriate function
    //  average += ACS.mA_AC_sampling();
    average += ACS.mA_AC();
  }
  float mA = average / 100.0;
  uint32_t duration = millis() - start;
  Serial.print("Time: ");
  Serial.print(duration);
  Serial.print("  mA: ");
  Serial.println(mA);
  scale = mA/725;
  norm = scale * 255;
  Serial.print("scaled_ vale ");
  Serial.print(norm);
  analogWrite(ledPin, norm );

  delay(1000);
}


// -- END OF FILE --
