#pragma once
#include "Config.h"

// =========================
// BNO055 IMU SETUP
// =========================
TwoWire I2C_2 = TwoWire(1);
Adafruit_BNO055 bno(55, BNO055_ADDRESS_B, &I2C_2);
uint16_t BNO055_SAMPLERATE_DELAY_MS = 100;

void initIMU() {
  I2C_2.begin(16, 17);
  Serial.println("Initializing BNO055...");
  if (!bno.begin()) {
    Serial.println("BNO055 not detected!");
    while (1) delay(500);
  }
  Serial.println("BNO055 detected!");
}

void sendIMUOverUDP() {
  // ===== BNO055 READ ===== (orientation, angular velocity, linear accel)
  sensors_event_t o, g, l;
  bno.getEvent(&o, Adafruit_BNO055::VECTOR_EULER);
  bno.getEvent(&g, Adafruit_BNO055::VECTOR_GYROSCOPE);
  bno.getEvent(&l, Adafruit_BNO055::VECTOR_LINEARACCEL);

  // Format as CSV string for easy parsing
  String imuPacket =
    String(o.orientation.x, 2) + "," +
    String(o.orientation.y, 2) + "," +
    String(o.orientation.z, 2) + ";";

  // PRINT TO SERIAL
  Serial.print("IMU → ");
  Serial.println(imuPacket);

  // Send over UDP
  udp.beginPacket(udpAddress, REMOTE_PORT);
  udp.print(imuPacket);
  udp.endPacket();

  delay(BNO055_SAMPLERATE_DELAY_MS);
}