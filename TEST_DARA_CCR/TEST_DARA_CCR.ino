uint8_t pipInputBuf[500];

int pipInputPointer = 0;

// Structure to store the data from the PIP4048
struct pipVals_t {
  char gridVoltage[16];
  char gridFrequency[16];
  char acOutput[16];
  char acFrequency[16];
  char acApparentPower[16];
  char acActivePower[16];
  char loadPercent[16];
  char busVoltage[16];
  char batteryVoltage[16];
  char batteryChargeCurrent[16];
  char batteryCharge[16];
  char inverterTemperature[16];
  char PVCurrent[16];
  char PVVoltage[16];
} pipVals;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); //Serial Monitor Console Baud Setting
  Serial1.begin(2400, SERIAL_8N1); //MPP-Solar inverter Baud Setting(http://www.offgrid.casa/wp-content/uploads/2017/10/HS_MS_MSX_RS232_Protocol_20140822_after_current_upgrade.pdf)

}

void loop() {
  // put your main code here, to run repeatedly:
  uint16_t crc;
  char *val;
  char pipstatus[40];

  delay(5000); // read every 5 

  int i = processPipInput(&crc);
  if (i > 0) // Got a good packet
  {
    Serial.println("Got good packet!");
    Serial.println((char *) pipInputBuf);

        val = strtok((char *) pipInputBuf, " "); // get the first value
        strcpy(pipVals.gridVoltage, val + 1); // Skip the initial '('

        val = strtok(0, " "); // Get the next value
        strcpy(pipVals.gridFrequency, val);

        val = strtok(0, " "); // Get the next value
        strcpy(pipVals.acOutput, val);

        val = strtok(0, " "); // Get the next value
        strcpy(pipVals.acFrequency, val);

        val = strtok(0, " "); // Get the next value
        strcpy(pipVals.acApparentPower, val);

        val = strtok(0, " "); // Get the next value
        strcpy(pipVals.acActivePower, val);

        val = strtok(0, " "); // Get the next value
        strcpy(pipVals.loadPercent, val);

        val = strtok(0, " "); // Get the next value
        strcpy(pipVals.busVoltage, val);

        val = strtok(0, " "); // Get the next value
        strcpy(pipVals.batteryVoltage, val);

        val = strtok(0, " "); // Get the next value
        strcpy(pipVals.batteryChargeCurrent, val);

        val = strtok(0, " "); // Get the next value
        strcpy(pipVals.batteryCharge, val);

        val = strtok(0, " "); // Get the next value
        strcpy(pipVals.inverterTemperature, val);

        val = strtok(0, " "); // Get the next value
        strcpy(pipVals.PVCurrent, val);

        val = strtok(0, " "); // Get the next value
        strcpy(pipVals.PVVoltage, val);


        // Print out readings
        Serial.print("gridVoltage: ");
        Serial.println(pipVals.gridVoltage);
        Serial.print("gridFrequency: ");
        Serial.println(pipVals.gridFrequency);
        Serial.print("acOutput: ");
        Serial.println(pipVals.acOutput);
        Serial.print("acFrequency: ");
        Serial.println(pipVals.acFrequency);
        Serial.print("acApparentPower: ");
        Serial.println(pipVals.acApparentPower);
        Serial.print("acActivePower: ");
        Serial.println(pipVals.acActivePower);
        Serial.print("loadPercent: ");
        Serial.println(pipVals.loadPercent);
        Serial.print("busVoltage: ");
        Serial.println(pipVals.busVoltage);
        Serial.print("batteryVoltage: ");
        Serial.println(pipVals.batteryVoltage);
        Serial.print("batteryChargeCurrent: ");
        Serial.println(pipVals.batteryChargeCurrent);
        Serial.print("batteryCharge: ");
        Serial.println(pipVals.batteryCharge);
        Serial.print("inverterTemperature: ");
        Serial.println(pipVals.inverterTemperature);
        Serial.print("PVCurrent: ");
        Serial.println(pipVals.PVCurrent);
        Serial.print("PVVoltage: ");
        Serial.println(pipVals.PVVoltage);

        // Calculate PV Power
        int I, V;
        I = atoi(pipVals.PVCurrent);
        V = atoi(pipVals.PVVoltage);
        Serial.print("PVPower: ");
        Serial.println(I * V);
    }
  }



int processPipInput(uint16_t *retCrc)
{
  uint8_t pipChar;
  uint16_t newCrc;
  Serial.print("got to reading data");
  while (Serial1.available()) // Got any input?
  {
    if ((pipChar = Serial1.read()) != 0x0d) // Read the byte
    {
      pipInputBuf[pipInputPointer++] = pipChar; // Not a <cr>
      Serial.print("data not ccr");
    }
    else
    { // Got a <cr>, calculate the crc
      newCrc = cal_crc_half(pipInputBuf, pipInputPointer - 2);
      if (newCrc == ((((pipInputBuf[pipInputPointer - 2]) << 8) & 0xff00) | (pipInputBuf[pipInputPointer - 1] & 0xff))) // Good crc
      {
        int8_t i = pipInputPointer - 2;
        pipInputBuf[i] = 0; // Terminate the string in the input buffer, overwriting the crc - so it can  easily be printed out
        pipInputPointer = 0; // Zero the pointer ready for the next packet
        *retCrc = newCrc; // Return the buffer CRC
        Serial.print("data is new ccr");
        return (i); // Return length of buffer
      }
      else
      {
        pipInputBuf[pipInputPointer + 1] = 0; // Terminate the string for display...keep the crc in place for checking
        pipInputPointer = 0;
        return (-1); // Indicate bad crc
        Serial.print("data is not new  ccr");
        
      }
    }
  }

  return (0); // packet not yet finished
}

uint16_t cal_crc_half(uint8_t  *pin, uint8_t len)
{

  uint16_t crc;

  uint8_t da;
  uint8_t  *ptr;
  uint8_t bCRCHign;
  uint8_t bCRCLow;

  uint16_t crc_ta[16] =
  {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,

    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef
  };
  ptr = pin;
  crc = 0;

  while (len-- != 0)
  {
    da = ((uint8_t)(crc >> 8)) >> 4;

    crc <<= 4;

    crc ^= crc_ta[da ^ (*ptr >> 4)];

    da = ((uint8_t)(crc >> 8)) >> 4;

    crc <<= 4;

    crc ^= crc_ta[da ^ (*ptr & 0x0f)];

    ptr++;
  }
  bCRCLow = crc;

  bCRCHign = (uint8_t)(crc >> 8);

  if (bCRCLow == 0x28 || bCRCLow == 0x0d || bCRCLow == 0x0a)

  {
    bCRCLow++;
  }
  if (bCRCHign == 0x28 || bCRCHign == 0x0d || bCRCHign == 0x0a)

  {
    bCRCHign++;
  }
  crc = ((uint8_t)bCRCHign) << 8;
  crc += bCRCLow;
  return (crc);
}
