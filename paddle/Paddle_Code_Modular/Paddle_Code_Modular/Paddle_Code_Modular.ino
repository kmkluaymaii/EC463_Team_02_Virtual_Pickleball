#include "Config.h"
#include "Audio.h"
#include "Haptics.h"
#include "LEDs.h"
#include "IMU.h"
#include "UDPHelpers.h"

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== ESP32 AUDIO + HAPTIC UNIT BOOTING ===");

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(400);
  }
  Serial.println("\nConnected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  udp.begin(UDP_PORT);
  Serial.printf("Listening on UDP port %d\n", UDP_PORT);

  Wire.begin(SDA_PIN, SCL_PIN);
  delay(100);

  initHaptics();
  setupI2S();
  initLEDs();
  initIMU();
}

void loop() {
  sendIMUOverUDP();
  processAudio();
  processUDPCommands();
}
