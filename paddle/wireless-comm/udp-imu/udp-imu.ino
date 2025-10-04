#include <WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

// ========= WiFi / UDP Settings =========
const char* ssid = "BU Guest (unencrypted)";
const char* password = "";
const char* udpAddress = "168.122.149.186";   // <-- Replace with your receving PC’s IP
const int udpPort = 4210;

WiFiUDP udp;

// ========= IMU Settings =========
uint16_t BNO055_SAMPLERATE_DELAY_MS = 100;
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x29, &Wire);

void setup() {
  Serial.begin(115200);

  // Connect WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  // Start IMU
  if (!bno.begin()) {
    Serial.println("Ooops, no BNO055 detected ... Check wiring or I2C ADDR!");
    while (1);
  }
  delay(1000);
}

void loop() {
  // Grab IMU data
  sensors_event_t orientationData, angVelocityData, linearAccelData;
  bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);
  bno.getEvent(&angVelocityData, Adafruit_BNO055::VECTOR_GYROSCOPE);
  bno.getEvent(&linearAccelData, Adafruit_BNO055::VECTOR_LINEARACCEL);

  // bno.getEvent(&magnetometerData, Adafruit_BNO055::VECTOR_MAGNETOMETER);
  // bno.getEvent(&accelerometerData, Adafruit_BNO055::VECTOR_ACCELEROMETER);
  // bno.getEvent(&gravityData, Adafruit_BNO055::VECTOR_GRAVITY);

  // Format as CSV string for easy parsing
  String packet = "";
  //packet += "Orient:";
  packet += String(orientationData.orientation.x, 2) + ",";
  packet += String(orientationData.orientation.y, 2) + ",";
  packet += String(orientationData.orientation.z, 2) + ";";

  // packet += "Gyro:";
  // packet += String(angVelocityData.gyro.x, 2) + ",";
  // packet += String(angVelocityData.gyro.y, 2) + ",";
  // packet += String(angVelocityData.gyro.z, 2) + ";";

  // packet += "LinearAccel:";
  // packet += String(linearAccelData.acceleration.x, 2) + ",";
  // packet += String(linearAccelData.acceleration.y, 2) + ",";
  // packet += String(linearAccelData.acceleration.z, 2);

  // Send over UDP
  udp.beginPacket(udpAddress, udpPort);
  udp.print(packet);
  udp.endPacket();

  // Also print to Serial for debugging
  Serial.println(packet);

  delay(BNO055_SAMPLERATE_DELAY_MS);
}