int incomingByte = 0; // for incoming serial data
uint8_t inverter ;
void setup() {
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
  Serial1.begin(1200);
  
}

void loop() {
  // send data only when you receive data:
  if (Serial1.available() > 0) {
    // read the incoming byte:
    inverter = Serial1.read();

    // say what you got:
    Serial.print("I received: ");
    Serial.println(inverter,DEC);
    delay(100);
  }
}
