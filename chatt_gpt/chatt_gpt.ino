#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Anvil";
const char* password = "1234567890";
const char* mqttServer = "172.105.95.116";
const int mqttPort = 1900;
const char* mqttUser = "";
const char* mqttPassword = "";

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  client.setServer(mqttServer, mqttPort);
}

void loop() {
  if (!client.connected()) {
    if (client.connect("", mqttUser, mqttPassword)) {
      Serial.println("Connected to MQTT server");
    } else {
      Serial.println("Failed to connect to MQTT server");
      delay(1000);
    }
  }
  client.loop();
}
