#include <Wire.h>
#include "INA219.h"

#ifdef SERIAL_USB
  #define CONSOLE SerialUSB
#else
  #define CONSOLE Serial
#endif

INA219 INA(0x40);  // INA219 I2C address (default)

void setup()
{
  // Use USB CDC Serial
  SerialUSB.begin();
  while (!SerialUSB);  // Wait for USB serial to connect

  Wire.begin();  // Initialize I2C

  SerialUSB.println("Starting INA219 Sensor...");
  if (!INA.begin())
  {
    SerialUSB.println("Could not connect to INA219. Halting.");
    while (1);  // Stop here if sensor not found
  }

  INA.setMaxCurrentShunt(0.8, 0.1);  // Max expected current and shunt resistance (Ohms)
  delay(1000);

  // CSV header
  SerialUSB.println("BUS_V\tSHUNT_mV\tCURRENT_mA\tPOWER_mW\tOVF\tCNVR");
}

void loop()
{
  const int samples = 10;
  float busV = 0, shuntmV = 0, currentmA = 0, powermW = 0;

  for (int i = 0; i < samples; i++) {
    busV     += INA.getBusVoltage();
    shuntmV  += INA.getShuntVoltage_mV();
    currentmA+= INA.getCurrent_mA();
    powermW  += INA.getPower_mW();
    delay(50);
  }

  busV     /= samples;
  shuntmV  /= samples;
  currentmA/= samples;
  powermW  /= samples;

  bool ovf  = INA.getMathOverflowFlag();
  bool cnvr = INA.getConversionFlag();

  SerialUSB.print(busV, 3);      SerialUSB.print("\t");
  SerialUSB.print(shuntmV, 3);   SerialUSB.print("\t");
  SerialUSB.print(currentmA, 3); SerialUSB.print("\t");
  SerialUSB.print(powermW, 3);   SerialUSB.print("\t");
  SerialUSB.print(ovf);          SerialUSB.print("\t");
  SerialUSB.println(cnvr);

  delay(500);
}

