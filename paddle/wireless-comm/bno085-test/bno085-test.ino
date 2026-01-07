
// STEP 1: install library: Adafruit BNO08x

#include <Adafruit_BNO08x.h>

// wiring from bno085 to the esp32
const unsigned int RST_PIN = 5; // rst = gpio36
const unsigned int INT_PIN = 9;
const unsigned int CS_PIN = 10;
const unsigned int ADD_PIN = 23;
const unsigned int SDA_PIN = 21; // NOT the same as the 22/21 SDA/SCL for I2C
const unsigned int SCL_PIN = 22; // NOT the same as the 22/21 SDA/SCL for I2C

const unsigned int DELAY_MS = 10;

sh2_SensorValue_t sensorValue;

// reset the bno
Adafruit_BNO08x bno08x(RST_PIN);

void setReports(void) {
  Serial.println("Setting reports...");
  if (!bno08x.enableReport(SH2_GAME_ROTATION_VECTOR)) {
    Serial.println("Uh oh, failed to enable vector.");
  }
}

void setup() {
  // set up serial monitor
  Serial.begin(115200);
  Serial.println("HELLLOOOOOOOOOOOOOOOOOOOOOOO");
  while (!Serial) {
    Serial.println("Uh oh, no serial...");
    delay(1000);
  }

  // attempt to initialize the bno
  if (!bno08x.begin_SPI(CS_PIN, INT_PIN)) {
    Serial.println("Uh oh, can't find the BNO08x chip :(");
    // give up
    while (1) { delay(10); }
  }
  Serial.println("Found the BNO08x.");

}


void loop() {
  delay(DELAY_MS);
/*
  if (bno08x.wasReset()) {
    Serial.print("Sensor was reset ");
    setReports();
  }

  if (!bno08x.getSensorEvent(&sensorValue)) {
    return;
  }

  switch(sensorValue.sensorId) {
    case SH2_GAME_ROTATION_VECTOR:
      // Serial.print("game rotation vector r: ");
      // Serial.print(sensorValue.un.gameRotationVector.real);
      // Serial.print(" i: ");
      // Serial.print(sensorValue.un.gameRotationVector.i);
      // Serial.print(" j: ");
      // Serial.print(sensorValue.un.gameRotationVector.j);
      // Serial.print(" k: ");
      // Serial.println(sensorValue.un.gameRotationVector.k);
      break;
  }
  */
}






