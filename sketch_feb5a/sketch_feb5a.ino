#include <WiFi.h>
#include <MQTT.h>


const char* ssid = "Anvil";
const char* password = "1234567890";
unsigned long lastmillis = 0 ;

WiFiClient net;
MQTTClient client;

void connect(){
  Serial.print("Cheking wifi status.....");
  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting......");

  while(!client.connect("29c3f12466b54850a985489d111baaf9.s1.eu.hivemq.cloud","onetwo","1234567890"))
  {
  Serial.print(".");
  delay(1000);
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin (115200);
  WiFi.begin(ssid,password);

  client.begin("29c3f12466b54850a985489d111baaf9.s1.eu.hivemq.cloud",net);
  connect();
}

void loop() {
  // put your main code here, to run repeatedly:
  client.loop();
  delay(10);
  if(!client.connected())
  {
    connect();
  }
  if (millis() - lastmillis > 15000)
  {
    lastmillis = millis();
  }
  client.publish("/temp",String(10));
  client.publish("/hum",String(20));
}
