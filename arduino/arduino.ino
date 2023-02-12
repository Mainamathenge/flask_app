#include <WiFi.h>
#include <HTTPClient.h>
#include <Q2HX711.h>
 
const byte hx711_data_pin = 16;
const byte hx711_clock_pin = 4;
Q2HX711 hx711(hx711_data_pin,hx711_clock_pin);

const char* ssid = "Anvil";
const char* password = "1234567890";
const char* server = "http://52.36.201.129/cbes/lux";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  //float temperature = 25.0;   // Example temperature value
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
 

  HTTPClient http;
  http.begin(server);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  //String data = "&temperature=" + String(temperature) + "&humidity=" + String(humidity);
  String data = "&temperature=" + String(iradiance) ;
  int httpResponseCode = http.POST(data);

  if (httpResponseCode > 0) {
    Serial.println("Data sent successfully");
  } else {
    Serial.println("Error sending data");
  }
  http.end();
  delay(30000);
}