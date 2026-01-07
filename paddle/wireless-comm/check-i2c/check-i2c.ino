#include <Wire.h>

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("I2C Scanner starting...");

  // Use the sda scl pin on your board
  Wire.begin(21,22); // SDA, SCL
  scanI2C();
}

void loop() {}

void scanI2C() {
  byte error, address;
  int nDevices = 0;

  Serial.println("Scanning I2C bus...");

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      Serial.println(address, HEX);
      nDevices++;
    }
  }

  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("Scan complete\n");

}