#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

// Create BNO055 object
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire); // Default I2C address is 0x28

void setup() {
  Serial.begin(115200);
  delay(1000);
  Wire.begin(8,9);

  // Initialize BNO055
  if (!bno.begin()) {
    Serial.println("Failed to initialize BNO055! Check wiring or I2C address.");
    while (1);
  }
  Serial.println("BNO055 initialized successfully!");

  // Optional: Use external crystal for better accuracy
  bno.setExtCrystalUse(true);
}

void loop() {
  // Get rotation (Euler angles)
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);

  // Get linear acceleration (m/s^2)
  imu::Vector<3> linAccel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);

  // Print data
  Serial.print("Rotation (X, Y, Z) [deg]: ");
  Serial.print(euler.x());
  Serial.print(", ");
  Serial.print(euler.y());
  Serial.print(", ");
  Serial.print(euler.z());
  Serial.print(" | ");

  Serial.print("Linear Accel (X, Y, Z) [m/s^2]: ");
  Serial.print(linAccel.x());
  Serial.print(", ");
  Serial.print(linAccel.y());
  Serial.print(", ");
  Serial.println(linAccel.z());

  delay(100); // Sampling delay
}
