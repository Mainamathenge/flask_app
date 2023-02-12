 #include <Q2HX711.h>
 
const byte hx711_data_pin = 16;
const byte hx711_clock_pin = 4;
Q2HX711 hx711(hx711_data_pin,hx711_clock_pin);

void setup() {
  Serial.begin(9600);

}
void loop() {
  float raw = 0;
  float data2 = 0;
  float mili_volt = 0;
  float iradiance = 0;

  raw = hx711.read()/100.0;
  data2 = raw - 85986;
  mili_volt = data2/2600.859;
  iradiance = (mili_volt/10.4)*1000;
  //Serial.println(iradiance);
  Serial.println(mili_volt);
  //Serial.println(hx711.read()/100.0);
  delay(5000);

  
}
