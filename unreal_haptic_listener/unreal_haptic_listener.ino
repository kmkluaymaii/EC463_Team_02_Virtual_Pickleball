#include <WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <Adafruit_DRV2605.h>

const char* ssid = "cc";
const char* password = "crinklecoffee";

WiFiUDP udp;
const int udpPort = 5005;
char incomingPacket[2048];

Adafruit_DRV2605 drv0;   // DRV2605 on mux channel 0
Adafruit_DRV2605 drv1;   // DRV2605 on mux channel 1

// ---- SELECT TCA9548A CHANNEL ----
void tcaSelect(uint8_t channel) {
  Wire.beginTransmission(0x70);   // TCA9548A default address
  Wire.write(1 << channel);       // Enable channel
  Wire.endTransmission();
}

void setupDriver(Adafruit_DRV2605 &drv, uint8_t channel) {
  tcaSelect(channel);        // Enable channel on mux
  if (!drv.begin()) {
    Serial.print("❌ DRV2605 failed on channel ");
    Serial.println(channel);
    while (1);
  }

  drv.selectLibrary(1);
  drv.setMode(DRV2605_MODE_INTTRIG);

  Serial.print("DRV2605 initialized on channel ");
  Serial.println(channel);
}

void vibrateDriver(Adafruit_DRV2605 &drv, uint8_t channel, uint8_t effect) {
  tcaSelect(channel);      // Switch mux to the right driver

  drv.setWaveform(0, effect);  // choose an effect
  drv.setWaveform(1, 0);       // end waveform list
  drv.go();                    // trigger vibration
}

// ========== SETUP ==========
void setup() {
  Serial.begin(115200);
  delay(500);

  // ---- I2C on ESP32 pins 8 (SDA) and 9 (SCL) ----
  Wire.begin(8, 9);

  // ---- Initialize DRV2605 Motors ----
  setupDriver(drv0, 0);   // DRV on channel 0
  setupDriver(drv1, 1);   // DRV on channel 1

  // ---- WiFi ----
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.println(WiFi.localIP());

  // ---- UDP ----
  udp.begin(udpPort);
  Serial.print("🎧 Listening for UDP on port ");
  Serial.println(udpPort);
}

// ========== MAIN LOOP ==========
void loop() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    int len = udp.read(incomingPacket, 2047);
    incomingPacket[len] = '\0';

    float x, y, z;
    if (sscanf(incomingPacket, "%f,%f,%f", &x, &y, &z) == 3) {
      Serial.printf("X: %.3f, Y: %.3f, Z: %.3f\n", x, y, z);

      // ---- Example Logic: Use X to pick motor ----
      if (x) {
        vibrateDriver(drv0, 0, 47);   // left side
        vibrateDriver(drv1, 1, 47);   // right side
      }
    }
  }

  delay(10);
}
